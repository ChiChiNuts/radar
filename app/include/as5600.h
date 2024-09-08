/*
 * as5600.h
 *
 *  Created on: Sep 6, 2024
 *      Author: wormarz
 */

#ifndef INCLUDE_AS5600_H_
#define INCLUDE_AS5600_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

int as5600_init(void);
uint8_t get_as5600_status(void);
uint16_t get_as5600_angle(uint8_t is_raw);
void as5600_dump(void);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_AS5600_H_ */
