//
// Created by bluse on 2022/7/31.
//

#include <string.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <stdlib.h>
#include "handler.h"

int max_delay = 60;
static mosquitto_plugin_id_t *mos_pid = NULL;

int mosquitto_plugin_version(int supported_version_count, const int *supported_versions) {
    int i;

    for (i = 0; i < supported_version_count; i++) {
        if (supported_versions[i] == 5) {
            return 5;
        }
    }
    return -1;
}

int
mosquitto_plugin_init(mosquitto_plugin_id_t *identifier, void **user_data, struct mosquitto_opt *opts, int opt_count) {
    for (int i = 0; i < opt_count; ++i) {
        mosquitto_log_printf(MOSQ_LOG_DEBUG, "%s -> %s %d", opts->key, opts->value, strcmp(opts->key, "max_delay"));
        if (strcmp(opts->key, "max_delay") == 0) {
            max_delay = atoi(opts->value);
            if (max_delay > 600 || max_delay < 6) {
                return MOSQ_ERR_UNKNOWN;
            }
            mosquitto_log_printf(MOSQ_LOG_DEBUG, "maxDelay -> %d", max_delay);
        }
        opts++;
    }

    mos_pid = identifier;
    int rs = mosquitto_callback_register(mos_pid, MOSQ_EVT_TICK, handle_delay_message_tick, NULL, NULL);
    if (rs != MOSQ_ERR_SUCCESS) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "mosquitto_callback_register MOSQ_EVT_TICK err:%d", rs);
        return rs;
    }
    rs = mosquitto_callback_register(mos_pid, MOSQ_EVT_MESSAGE, handle_delay_message, NULL, NULL);
    if (rs != MOSQ_ERR_SUCCESS) {
        mosquitto_log_printf(MOSQ_LOG_ERR, "mosquitto_callback_register MOSQ_EVT_MESSAGE err:%d", rs);
        return rs;
    }
    return MOSQ_ERR_SUCCESS;
}


int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count) {
    if (mos_pid) {
        int rs = mosquitto_callback_unregister(mos_pid, MOSQ_EVT_TICK, handle_delay_message_tick, NULL);
        if (rs != MOSQ_ERR_SUCCESS) {
            mosquitto_log_printf(MOSQ_LOG_ERR, "mosquitto_callback_unregister MOSQ_EVT_MESSAGE err:%d", rs);
            return rs;
        }
        rs = mosquitto_callback_unregister(mos_pid, MOSQ_EVT_MESSAGE, handle_delay_message, NULL);
        if (rs != MOSQ_ERR_SUCCESS) {
            mosquitto_log_printf(MOSQ_LOG_ERR, "mosquitto_callback_unregister MOSQ_EVT_MESSAGE err:%d", rs);
            return rs;
        }
    }
    return MOSQ_ERR_SUCCESS;
}
