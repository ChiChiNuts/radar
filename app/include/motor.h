/*
 * motor.h
 *
 *  Created on: Jun 8, 2024
 *      Author: wormarz
 */

#ifndef INCLUDE_MOTOR_H_
#define INCLUDE_MOTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

int step_motor_init(void);
void step_clockwise(uint32_t ms);
void step_counterclockwise(uint32_t ms);
void step_motor_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_MOTOR_H_ */
