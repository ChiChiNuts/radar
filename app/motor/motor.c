/*
 * motor.c
 *
 *  Created on: Jun 8, 2024
 *      Author: wormarz
 */
#include "misc.h"
#include "gpio.h"
#include "tim.h"

#define PHASE_A(x)         HAL_GPIO_WritePin(SMPA_GPIO_Port, SMPA_Pin, x)
#define PHASE_B(x)         HAL_GPIO_WritePin(SMPB_GPIO_Port, SMPB_Pin, x)
#define PHASE_C(x)         HAL_GPIO_WritePin(SMPC_GPIO_Port, SMPC_Pin, x)
#define PHASE_D(x)         HAL_GPIO_WritePin(SMPD_GPIO_Port, SMPD_Pin, x)

#define PHASE_A_LOW        PHASE_A(GPIO_PIN_RESET)
#define PHASE_A_HIGH       PHASE_A(GPIO_PIN_SET)

#define PHASE_B_LOW        PHASE_B(GPIO_PIN_RESET)
#define PHASE_B_HIGH       PHASE_B(GPIO_PIN_SET)

#define PHASE_C_LOW        PHASE_C(GPIO_PIN_RESET)
#define PHASE_C_HIGH       PHASE_C(GPIO_PIN_SET)

#define PHASE_D_LOW        PHASE_D(GPIO_PIN_RESET)
#define PHASE_D_HIGH       PHASE_D(GPIO_PIN_SET)

#define PA_MASK            0x01
#define PB_MASK            0x02
#define PC_MASK            0x04
#define PD_MASK            0x08

#define MOTOR_MS_TO_TICKS(x)  ((x) * 4 - 1)

static uint8_t step_index;
static uint8_t is_cw;
const uint8_t motor_steps[] = {
        0x01, 0x03, 0x02, 0x06,
        0x04, 0x0c, 0x08, 0x09,
};

static volatile uint8_t is_working;

int step_motor_init(void)
{
    return 0;
}

void step_clockwise(uint32_t ms)
{
    is_cw = 1;
    step_index = 0;
    __HAL_TIM_SET_AUTORELOAD(&htim10, MOTOR_MS_TO_TICKS(ms));
    HAL_TIM_Base_Start_IT(&htim10);
    is_working = 1;
}

void step_counterclockwise(uint32_t ms)
{
    is_cw = 0;
    step_index = 7;
    __HAL_TIM_SET_AUTORELOAD(&htim10, MOTOR_MS_TO_TICKS(ms));
    HAL_TIM_Base_Start_IT(&htim10);
    is_working = 1;
}

void step_motor_stop(void)
{
    HAL_TIM_Base_Stop_IT(&htim10);

    PHASE_A_LOW;
    PHASE_B_LOW;
    PHASE_C_LOW;
    PHASE_D_LOW;

    is_working = 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim11.Instance) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    } else if (htim->Instance == htim10.Instance) {
        (PA_MASK & motor_steps[step_index]) ? PHASE_A_HIGH : PHASE_A_LOW;
        (PB_MASK & motor_steps[step_index]) ? PHASE_B_HIGH : PHASE_B_LOW;
        (PC_MASK & motor_steps[step_index]) ? PHASE_C_HIGH : PHASE_C_LOW;
        (PD_MASK & motor_steps[step_index]) ? PHASE_D_HIGH : PHASE_D_LOW;

        if (is_cw) {
            step_index = (step_index + 1) % 8;
        } else {
            step_index = step_index ? (step_index - 1) : 7;
        }
    }
}
