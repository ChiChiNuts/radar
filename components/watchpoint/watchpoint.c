/*
 * watchpoint.c
 *
 *  Created on: Mar 23, 2024
 *      Author: wormarz
 */
#include "sys_reg.h"

#define sysreg_read(_r) (*(volatile unsigned int *)(_r))
#define sysreg_write(_r, _x) (*(volatile unsigned int *)(_r) = (_x))

unsigned int watchpoint_init(void)
{
	sysreg_write(DEMCR, DWT_MON_EN | sysreg_read(DEMCR));
	return sysreg_read(DWT_INFO_REG);
}

void watchpoint_deinit(void)
{
	sysreg_write(DEMCR, (~DWT_MON_EN) & sysreg_read(DEMCR));
}

void register_watchpoint(void *addr)
{
	unsigned int dbg_reg = sysreg_read(DEMCR);
	if (!(dbg_reg & DWT_MON_EN)) {
		sysreg_write(DEMCR, dbg_reg | DWT_MON_EN);
	}
	/**configure resister**/
	sysreg_write(DWT_COMP0, (unsigned int)addr);
	sysreg_write(DWT_MASK0, 0);
	/**enable watchpoint**/
	sysreg_write(DWT_FUNCTION0, DWT_MONITOR_RW);
}

void unregister_watchpoint(void *addr)
{
	/**clear register**/
	sysreg_write(DWT_FUNCTION0, 0xFFFFFFF0 & sysreg_read(DWT_FUNCTION0));
	/**disable watchpoint**/

}
