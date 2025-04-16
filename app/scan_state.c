/*
 * scan_state.c
 *
 *  Created on: Oct 20, 2024
 *      Author: wormarz
 */
#include "scan_state.h"
#include "elog.h"
#include "usart.h"
#include "radar_hal.h"

#undef LOG_TAG
#define LOG_TAG    "scan_state"

#define MAX_ANGLE 179
#define MIN_ANGLE 0

static uint16_t cur_distance;
static uint16_t cur_angle;
static bool is_ccw;

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

void subscan_ranging_init(struct working_state *state)
{
    UNUSED(state);
    hal_distance_module_init();
}

void subscan_stepping_init(struct working_state *state)
{
    UNUSED(state);
    hal_motor_module_init(M10DPS);
}

void subscan_com_init(struct working_state *state)
{
    UNUSED(state);
    hal_com_module_init(B115200);
}

struct working_state* subscan_reposition(struct working_state *self)
{
    /* TODO:  reposition angle */
    cur_distance = 0;
    cur_angle = 0;
    CHG_STATE(self, "ranging");
    return self;
}

struct working_state* subscan_ranging(struct working_state *self)
{
    cur_distance = hal_distance_ranging();
    CHG_STATE(self, "stepping");
    return self;
}

struct working_state* subscan_stepping(struct working_state *self)
{
    if (cur_angle == MAX_ANGLE) {
        is_ccw = true;
    } else if (cur_angle == MIN_ANGLE) {
        is_ccw = false;
    }

    is_ccw ? --cur_angle : ++cur_angle;

    hal_motor_rotate(1, is_ccw);
    CHG_STATE(self, "com");
    return self;
}

struct working_state* subscan_com(struct working_state *self)
{
    hal_com_update(cur_distance, cur_angle + (is_ccw ? 1 : -1));
    char ch;
    log_i("into %s", self->state);

    if (HAL_UART_Receive(&huart1, (uint8_t*) &ch, 1, 0) == HAL_OK) {
        if (ch == 'I') {
            CHG_STATE(self, "idle");
        }
    } else {
        CHG_STATE(self, "ranging");
    }

    return self;
}
