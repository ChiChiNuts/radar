#ifndef	__MISC__
#define __MISC__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ARRAY_SIZE(arr)   (sizeof(arr)/sizeof((arr)[0]))
#define STR1(R) #R
#define STR2(R) STR1(R)

void delay_ms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif
