//
// Created by bluse on 2022/7/31.
//

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
        r->key = key;
        r->arr = malloc(sizeof(struct delay_message));
        r->arr[0] = msg;
        r->count = 1;

        HASH_ADD_STR(pData, key, r);
    } else {
        r->count += 1;
        struct delay_message *arr = calloc(r->count, sizeof(struct delay_message));
        for (int i = 0; i < r->count; ++i) {
            if (i < r->count - 1) {
                arr[i] = r->arr[i];
            } else {
                arr[i] = msg;
            }
        }
        free(r->arr);
        r->arr = arr;
    }
}

struct delay_message_array get(const char *key) {
    if (key == NULL) {
        return (struct delay_message_array) {NULL, 0};
    }
    struct dataItem *i;
    HASH_FIND_STR(pData, key, i);
    if (i == NULL) {
        return (struct delay_message_array) {NULL, 0};
    }
    struct delay_message_array arr = {i->arr, i->count};
    return arr;
}

void del(const char *key) {
    if (key == NULL) {
        return;
    }

    struct dataItem *r;
    HASH_FIND_STR(pData, key, r);
    if (r != NULL) {
        HASH_DEL(pData, r);
        // 完整清理
        for (int i = 0; i < r->count; ++i) {
            free(r->arr[i].topic);
            free(r->arr[i].payload);
        }
        free(r->arr);
        r->arr = NULL;
        free(r);
        r = NULL;
    }
}