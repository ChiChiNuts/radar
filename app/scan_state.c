/*
 * scan_state.c
 *
 *  Created on: Oct 20, 2024
 *      Author: wormarz
 */
#include "scan_state.h"
#include "elog.h"
#include "usart.h"

#undef LOG_TAG
#define LOG_TAG    "scan_state"

struct working_state* sub1_state(struct working_state *self)
{
    log_i("into %s", self->state);
    CHG_STATE(self, "sub2");

    return self;
}

struct working_state* scan_state(struct working_state *self)
{
    char ch;
    log_i("into %s", self->state);

    if (HAL_UART_Receive(&huart1, (uint8_t*) &ch, 1, 1000) == HAL_OK) {
        if (ch == 'I') {
            CHG_STATE(self, "idle");
        }
    }

    return self;
}

