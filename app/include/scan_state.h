/*
 * scan_state.h
 *
 *  Created on: Oct 20, 2024
 *      Author: wormarz
 */

#ifndef INCLUDE_SCAN_STATE_H_
#define INCLUDE_SCAN_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "misc.h"

struct working_state* sub1_state(struct working_state *self);
struct working_state* scan_state(struct working_state *self);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SCAN_STATE_H_ */
