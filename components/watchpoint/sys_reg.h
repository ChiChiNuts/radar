/*
 * sys_reg.h
 *
 *  Created on: Mar 24, 2024
 *      Author: wormarz
 */

#ifndef COMPONENTS_WATCHPOINT_SYS_REG_H_
#define COMPONENTS_WATCHPOINT_SYS_REG_H_

#define BIT(_val, _sft)   ((_val) << (_sft))

#if __ARM_ARCH == 7

#define DWT_MONITOR_W 	0b0110
#define DWT_MONITOR_R 	0b0101
#define DWT_MONITOR_RW 	0b0111

#define DWT_MON_EN		(BIT(0x1, 16) | BIT(0X1, 24))

#define DEMCR			0xE000EDFC
#define DWT_INFO_REG 	0xE0001000

#define DWT_COMP0 		0xE0001020
#define DWT_MASK0		0xE0001024
#define DWT_FUNCTION0	0xE0001028

#elif __ARM_ARCH == 8

#define DWT_INFO_REG 0x80200

#else

#error "Do not support current arch!!\n"

#endif

#endif /* COMPONENTS_WATCHPOINT_SYS_REG_H_ */
