/*
 * radar_hal.c
 *
 *  Created on: Oct 7, 2024
 *      Author: wormarz
 */
#include "radar_hal.h"
#include "misc.h"
#include "string.h"
#include "vl6180x.h"
#include "as5600.h"
#include "motor.h"
#include "usart.h"

/*******distance module*********/
const struct hal_param_k_v distance_params[] = {
        { .key = "unit", .val = "mm" },
        { .key = "range", .val = "100" },
};
void hal_distance_module_init(void)
{
    VL6180X_Init();
}
void hal_distance_module_deinit(void)
{
    ;
}
int hal_distance_get_param(char *key, const char **val)
{
    int size = ARRAY_SIZE(distance_params);
    for (int i = 0; i < size; ++i) {
        if (!strcmp(key, distance_params[i].key)) {
            *val = distance_params[i].val;
            return 0;
        }
    }

    return -1;
}
uint16_t hal_distance_ranging(void)
{
    VL6180x_Ranging();
    return VL6180_Read_Range();
}
/*******************************/

/*********angle module**********/
const struct hal_param_k_v angle_params[] = {
        { .key = "unit", .val = "degree" },
        { .key = "range", .val = "360" },
};
void hal_angle_module_init(void)
{
    as5600_init();
}
void hal_angle_module_deinit(void)
{
    ;
}
int hal_angle_get_param(char *key, const char **val)
{
    int size = ARRAY_SIZE(angle_params);
    for (int i = 0; i < size; ++i) {
        if (!strcmp(key, angle_params[i].key)) {
            *val = angle_params[i].val;
            return 0;
        }
    }

    return -1;
}
uint16_t hal_angle_get(void)
{
    return get_as5600_angle(1);
}
/*******************************/

/*********motor module**********/
const struct hal_param_k_v motor_params[] = {
        { .key = "unit", .val = "degree" },
        { .key = "speed", .val = "" },
};
void hal_motor_module_init(enum motor_speed speed)
{
    motor_init();
    switch (speed) {
        case M10DPS:
            set_motor_interval(8);
            break;
        case M20DPS:
            set_motor_interval(4);
            break;
        default:
            break;
    }
}
void hal_motor_module_deinit(void)
{
    ;
}
int hal_motor_get_param(char *key, const char **val)
{
    int size = ARRAY_SIZE(motor_params);
    for (int i = 0; i < size; ++i) {
        if (!strcmp(key, motor_params[i].key)) {
            *val = motor_params[i].val;
            return 0;
        }
    }

    return -1;
}
void hal_motor_rotate(uint16_t units, bool is_ccw)
{
    static uint16_t step = 11;

    for (int i = 0; i < units; ++i) {
        is_ccw ? step_counterclockwise(step) : step_clockwise(step);
        step = (step == 11) ? 12 : 11;
    }
}
/*******************************/
/*****communication module******/
const struct hal_param_k_v com_params[] = {
        { .key = "backend", .val = "uart" },
        { .key = "speed", .val = "115200" },
};
struct protocol_frame
{
    char pad1;
    char pad2;
    uint8_t reserved :5;
    uint8_t type :2;
    uint16_t angle;
    uint16_t distance;
} __attribute__((__packed__));

void hal_com_module_init(enum com_rate crate)
{
    UNUSED(crate);
}
void hal_com_module_deinit(void)
{
    ;
}
int hal_com_get_param(char *key, const char **val)
{
    int size = ARRAY_SIZE(com_params);
    for (int i = 0; i < size; ++i) {
        if (!strcmp(key, com_params[i].key)) {
            *val = com_params[i].val;
            return 0;
        }
    }

    return -1;
}

static struct protocol_frame data = {
        .pad1 = '#',
        .pad2 = '?',
};
int hal_com_update(uint16_t distance, uint16_t angle)
{
    data.angle = angle;
    data.distance = distance;
    if (HAL_UART_Transmit(&huart1, (uint8_t*) &data, sizeof(data), 0xFFFF)
            != HAL_OK) {
        return -1;
    }
    return 0;
}
/*******************************/
