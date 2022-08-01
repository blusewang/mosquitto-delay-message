//
// Created by bluse on 2022/7/31.
//

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include "delay_message.h"
#include "hash.h"

int cb_message(int event, void *event_data, void *userdata) {
    UNUSED(event);
    UNUSED(userdata);
    struct mosquitto_evt_message *msg = event_data;

    // 先把msg.topic数据换出来
    char *topic = malloc(strlen(msg->topic) + 1);
    strcpy(topic, msg->topic);

    // split topic
    char *topicSegment = strtok(topic, "/");
    // 判断是否为延迟消息
    if (strcmp(topicSegment, "delay") == 0) {
        // 读出延迟秒数
        topicSegment = strtok(NULL, "/");
        if (topicSegment == NULL) {
            free(topic);
            return MOSQ_ERR_SUCCESS;
        }

        long delay = atoi(topicSegment);

        // 筛选符合条件的延迟消息
        if (delay > max_delay || delay < 1) {
            free(topic);
            return MOSQ_ERR_SUCCESS;
        }

        // 开始构建延迟消息
        struct delay_message dm = {};
        dm.topic = malloc(strlen(msg->topic) + 1);
        dm.payload_len = msg->payloadlen;
        dm.payload = malloc(msg->payloadlen + 1);
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

        set(ctime(&ts), &dm);

//        mosquitto_log_printf(MOSQ_LOG_NOTICE, ">>>>> topic delay: %ld, %s, %s", delay, dm.topic, dm.payload);
    }
    free(topic);
    return MOSQ_ERR_SUCCESS;
}

int cb_tick(int event, void *event_data, void *userdata) {
    UNUSED(event);
    UNUSED(event_data);
    UNUSED(userdata);
    time_t ts;
    time(&ts);
    char *cts = ctime(&ts);
    struct delay_message_array arr = get(cts);
    if (arr.length > 0) {
//        mosquitto_log_printf(MOSQ_LOG_NOTICE, "cb_tick got len:%d, msg:%s", arr.length, arr.arr[0].topic);
        for (int i = 0; i < arr.length; ++i) {
            mosquitto_broker_publish_copy(NULL, arr.arr[i].topic, (int) strlen(arr.arr[i].payload) + 1, arr.arr[i].payload, arr.arr[i].qos, false, NULL);
        }

        del(ctime(&ts));
    }

    return MOSQ_ERR_SUCCESS;
}