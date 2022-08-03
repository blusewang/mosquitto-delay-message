//
// Created by bluse on 2022/7/31.
//

#ifndef MOSQUITTO_DELAY_MESSAGE_DELAY_MESSAGE_H
#define MOSQUITTO_DELAY_MESSAGE_DELAY_MESSAGE_H

#define UNUSED(A) (void)(A)

extern int max_delay;

int cb_tick(int event, void *event_data, void *userdata);

int cb_message(int event, void *event_data, void *userdata);

#endif //MOSQUITTO_DELAY_MESSAGE_DELAY_MESSAGE_H
