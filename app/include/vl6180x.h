#ifndef __VL6180X_H
#define __VL6180X_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int VL6180X_Init(void);
void VL6180x_Ranging(void);
uint8_t VL6180_Read_Range(void);
void VL6180X_Clear_Interrupt(void);

#ifdef __cplusplus
}
#endif

#endif  /* __VL6180X_H */
