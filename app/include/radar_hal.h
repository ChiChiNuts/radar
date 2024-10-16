/*
 * radar_hal.h
 *
 *  Created on: Oct 16, 2024
 *      Author: wormarz
 */

#ifndef INCLUDE_RADAR_HAL_H_
#define INCLUDE_RADAR_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

struct hal_param_k_v
{
    char *key;
    char *val;
};

/*******distance module*********/
void hal_distance_module_init(void);
void hal_distance_module_deinit(void);
int hal_distance_get_param(char *key, char *val);
uint16_t hal_distance_ranging(void);
/*******************************/

/*********angle module**********/
void hal_angle_module_init(void);
void hal_angle_module_deinit(void);
int hal_angle_get_param(char *key, char *val);
uint16_t hal_angle_get(void);
/*******************************/

/*********motor module**********/
void hal_motor_module_init(enum motor_speed speed);
void hal_motor_module_deinit(void);
int hal_motor_get_param(char *key, char *val);
void hal_motor_rotate(uint16_t units, bool is_ccw);
/*******************************/

/*****communication module******/
void hal_com_module_init(enum com_rate crate);
void hal_com_module_deinit(void);
int hal_com_get_param(char *key, char *val);
int hal_com_update(uint16_t distance, uint16_t angle);
/*******************************/
#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_RADAR_HAL_H_ */
