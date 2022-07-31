//
// Created by bluse on 2022/7/31.
//

#include <mosquitto_broker.h>
#include <mosquitto.h>
#include "hash.h"
#include "uthash.h"

void set(const char *key, struct delay_message msg) {
    if (key == NULL) {
        return;
    }
    struct dataItem *r;
    HASH_FIND_STR(pData, key, r);
    if (r == NULL) {
        r = malloc(sizeof(struct dataItem));
        strcpy(r->key, key);
        r->d_msg = msg;

        HASH_ADD_STR(pData, key, r);
    } else {
        r->d_msg = msg;
    }
}

struct delay_message *get(const char *key) {
    if (key == NULL) {
        return NULL;
    }
    struct dataItem *i;
    HASH_FIND_STR(pData, key, i);
    if (i == NULL) {
        return NULL;
    }
    return &i->d_msg;
}

void del(const char *key) {
    if (key == NULL) {
        return;
    }

    struct dataItem *r;
    HASH_FIND_STR(pData, key, r);
    if (r != NULL) {
        HASH_DEL(pData, r);
        free(r);
    }
}