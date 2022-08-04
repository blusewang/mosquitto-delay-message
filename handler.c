//
// Created by bluse on 2022/7/31.
//

#include <time.h>
#include <string.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include "handler.h"
#include "hash.h"

int handle_delay_message(int event, void *event_data, void *userdata) {
    UNUSED(event);
    UNUSED(userdata);
    struct mosquitto_evt_message *msg = event_data;

    // 先把msg.topic数据换出来
    char *origin_topic = mosquitto_malloc(strlen(msg->topic) + 1);
    strcpy(origin_topic, msg->topic);

    // split origin_topic
    char *topicSegment = strtok(origin_topic, "/");
    // 判断是否为延迟消息
    if (strcmp(topicSegment, "delay") == 0) {
        // 读出延迟秒数
        topicSegment = strtok(NULL, "/");
        if (topicSegment == NULL) {
            mosquitto_free(origin_topic);
            return MOSQ_ERR_SUCCESS;
        }

        long delay = atoi(topicSegment);

        // 筛选符合条件的延迟消息
        if (delay > max_delay || delay < 1) {
            mosquitto_free(origin_topic);
            return MOSQ_ERR_SUCCESS;
        }

        // 开始构建延迟消息
        struct delay_message dm = {};
        dm.topic = mosquitto_malloc(strlen(msg->topic) + 1);
        dm.topic[0] = '\0';
        dm.payload_len = msg->payloadlen;
        dm.payload = mosquitto_calloc(1, (size_t) (msg->payloadlen));
        memcpy(dm.payload, msg->payload, msg->payloadlen);
        dm.qos = msg->qos;

        time_t ts;
        time(&ts);
        ts += delay;

        topicSegment = strtok(NULL, "/");
        while (topicSegment != NULL) {
            strcat(dm.topic, topicSegment);
            topicSegment = strtok(NULL, "/");
            if (topicSegment != NULL) {
                strcat(dm.topic, "/");
            }
        }

        set(ctime(&ts), dm);

//        mosquitto_log_printf(MOSQ_LOG_NOTICE, ">>>>> cache in origin_topic delay: %ld, %s, %s", delay, dm.origin_topic, msg->payload);
    }
    mosquitto_free(origin_topic);
    return MOSQ_ERR_SUCCESS;
}

time_t tts;

int handle_delay_messge_tick(int event, void *event_data, void *userdata) {
    UNUSED(event);
    UNUSED(event_data);
    UNUSED(userdata);
    time_t ts;
    time(&ts);
    if (tts != ts) {
        tts = ts;
        char *cts = ctime(&ts);
        struct delay_message_array arr = get(cts);
        if (arr.length > 0) {
//            mosquitto_log_printf(MOSQ_LOG_NOTICE, "handle_delay_messge_tick got topic:%s, len:%d, msg:%s", arr.arr[0].topic,
//                                 strlen(arr.arr[0].topic),
//                                 arr.arr[0].payload);
            for (int i = 0; i < arr.length; ++i) {
                mosquitto_broker_publish_copy(NULL, arr.arr[i].topic, (int) strlen(arr.arr[i].payload) + 1,
                                              arr.arr[i].payload, arr.arr[i].qos, false, NULL);
            }

            del(ctime(&ts));
        }
    }

    return MOSQ_ERR_SUCCESS;
}