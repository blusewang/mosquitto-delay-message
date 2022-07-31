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
    char *topic = malloc(strlen(msg->topic));
    strcpy(topic, msg->topic);

    // split topic
    char *topicSegment = strtok(topic, "/");
    // 判断是否为延迟消息
    if (strcmp(topicSegment, "delay") == 0) {
        // 读出延迟秒数
        topicSegment = strtok(NULL, "/");
        if (topicSegment == NULL) {
            return MOSQ_ERR_SUCCESS;
        }

        long delay = atoi(topicSegment);

        // 筛选符合条件的延迟消息
        if (delay > max_delay || delay < 1) {
            return MOSQ_ERR_SUCCESS;
        }

        // 开始构建延迟消息
        struct delay_message dm = {};
        dm.topic = malloc(sizeof(msg->topic));
        dm.payload = calloc(1, msg->payloadlen + 1);
        memcpy(dm.payload, msg->payload, (size_t) msg->payloadlen);
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

//        mosquitto_log_printf(MOSQ_LOG_NOTICE, ">>>>> topic delay: %ld, %s, %d", delay, msg->topic, msg->payloadlen);
    }

    return MOSQ_ERR_SUCCESS;
}

int cb_tick(int event, void *event_data, void *userdata) {
    UNUSED(event);
    UNUSED(event_data);
    UNUSED(userdata);
    time_t ts;
    time(&ts);
    char *cts = ctime(&ts);
    struct delay_message *msg = get(cts);
    if (msg != NULL) {
        mosquitto_broker_publish(NULL, msg->topic, (int) strlen(msg->payload), msg->payload, msg->qos, false, NULL);
//        mosquitto_log_printf(MOSQ_LOG_NOTICE, "cb_tick got msg:%s", msg->topic);
        del(ctime(&ts));
    }

    return MOSQ_ERR_SUCCESS;
}