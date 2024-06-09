#if (STM32F401xC && USE_HAL_DRIVER)
#include "stm32f4xx_hal.h"

void delay_ms(unsigned int ms)
{
	HAL_Delay(ms);
}

#else

void delay_ms(unsigned int ms)
{
    while (ms--);
}

void delay_us(unsigned int us)
{
    while (us--);
}

#endif
