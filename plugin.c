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
            if (max_delay > 600) {
                return MOSQ_ERR_UNKNOWN;
            } else if (max_delay < 6) {
                return MOSQ_ERR_UNKNOWN;
            }
            mosquitto_log_printf(MOSQ_LOG_DEBUG, "maxDelay -> %d", max_delay);
        }
        opts++;
    }

    mos_pid = identifier;
    mosquitto_callback_register(mos_pid, MOSQ_EVT_TICK, cb_tick, NULL, NULL);
    mosquitto_callback_register(mos_pid, MOSQ_EVT_MESSAGE, cb_message, NULL, NULL);

    return MOSQ_ERR_SUCCESS;
}


int mosquitto_plugin_cleanup(void *user_data, struct mosquitto_opt *opts, int opt_count) {
    if (mos_pid) {
        mosquitto_callback_unregister(mos_pid, MOSQ_EVT_TICK, cb_tick, NULL);
        mosquitto_callback_unregister(mos_pid, MOSQ_EVT_MESSAGE, cb_message, NULL);
    }
    return MOSQ_ERR_SUCCESS;
}
