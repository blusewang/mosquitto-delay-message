//
// Created by bluse on 2022/7/31.
//

#ifndef DELAY_MESSAGE_HASH_H
#define DELAY_MESSAGE_HASH_H

#include "uthash.h"

struct delay_message {
    char *topic;
    void *payload;
    int payload_len;
    int qos;
};

struct delay_message_array {
    struct delay_message *arr;
    int length;
};

static struct dataItem {
    const char *key;        // key
    struct delay_message *arr;
    int count;
    UT_hash_handle hh;
} *pData = NULL;

void set(const char *key, struct delay_message msg);

struct delay_message_array get(const char *key);

void del(const char *key);

#endif //DELAY_MESSAGE_HASH_H
