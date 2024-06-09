/*
 * watchpoint.h
 *
 *  Created on: Mar 23, 2024
 *      Author: wormarz
 */

#ifndef COMPONENTS_WATCHPOINT_WATCHPOINT_H_
#define COMPONENTS_WATCHPOINT_WATCHPOINT_H_


unsigned int watchpoint_init(void);
void register_watchpoint(void *addr);
void unregister_watchpoint(void *addr);


#endif /* COMPONENTS_WATCHPOINT_WATCHPOINT_H_ */
