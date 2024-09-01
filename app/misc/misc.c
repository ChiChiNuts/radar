#include "misc.h"
#include <string.h>

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

/* state machine start */

static struct working_state states[MAX_SM_NUMS];
static struct working_state *cur_stat = &states[0];

void init_state_machine(void)
{
    memset(states, 0, sizeof(states));
}

int add_state(char *name, worker wkr, state_init init)
{
    for (int i = 0; i < MAX_SM_NUMS; ++i) {
        if (states[i].state[0] == '\0') {
            strncpy(states[i].state, name, MAX_SM_NAME_LEN);
            states[i].runner = wkr;
            if (init != NULL) {
                init(&states[i]);
            }
            return 0;
        }
    }

    return -1;
}

void state_machine_loop(void)
{
    while (1) {
        cur_stat = cur_stat->runner(cur_stat);
    }
}

void setup_first_state(char *name)
{
    CHG_STATE(cur_stat, name);
}

struct working_state* stat_lookup(char *name)
{
    for (int i = 0; i < MAX_SM_NUMS; ++i) {
        if (strncmp(states[i].state, name, MAX_SM_NAME_LEN) == 0) {
            return &states[i];
        }
    }

    return NULL;
}
/* state machine end */
