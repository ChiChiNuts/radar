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

struct working_state* subscan_reposition(struct working_state *self);
struct working_state* subscan_ranging(struct working_state *self);
void subscan_ranging_init(struct working_state *state);
struct working_state* subscan_stepping(struct working_state *self);
void subscan_stepping_init(struct working_state *state);
struct working_state* subscan_com(struct working_state *self);
void subscan_com_init(struct working_state *state);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SCAN_STATE_H_ */
