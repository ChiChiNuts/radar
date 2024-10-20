/*
 * cmd_state.h
 *
 *  Created on: Oct 20, 2024
 *      Author: wormarz
 */

#ifndef INCLUDE_CMD_STATE_H_
#define INCLUDE_CMD_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "misc.h"

void cmd_state_init(struct working_state *state);
struct working_state* cmd_state(struct working_state *self);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_CMD_STATE_H_ */
