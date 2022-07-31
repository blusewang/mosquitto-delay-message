//
// Created by bluse on 2022/7/31.
//

#ifndef DELAY_MESSAGE_HASH_H
#define DELAY_MESSAGE_HASH_H

#include "uthash.h"

struct delay_message {
    char *topic;
    void *payload;
    int qos;
};

static struct dataItem {
    char key[1024];        // key
    struct delay_message d_msg;
    UT_hash_handle hh;
} *pData = NULL;

void set(const char *key, struct delay_message msg);
struct delay_message *get(const char *key);
void del(const char *key);

#endif //DELAY_MESSAGE_HASH_H
