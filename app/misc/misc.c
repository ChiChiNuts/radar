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

int add_state(char *name, worker wkr, void (*init)(struct working_state *))
{
    for (int i = 0; i < MAX_SM_NUMS; ++i) {
        if (states[i].state[0] == '\0') {
            strncpy(states[i].state, name, MAX_SM_NAME_LEN);
            states[i].runner = wkr;
            memset(states[i].edges, 0xff, sizeof(uint8_t) * MAX_SM_EDGES_NUMS);
            if (init != NULL) {
                init(&states[i]);
            }
            return 0;
        }
    }

    return -1;
}

int add_trans_rule(char *from, char *to)
{
    struct working_state *fstat, *tostat;
    int ret = -1;
    fstat = stat_lookup(from);
    tostat = stat_lookup(to);

    assert(fstat && tostat);

    for (int i = 0; i < MAX_SM_EDGES_NUMS; ++i) {
        if (fstat->edges[i] == 0xff) {
            fstat->edges[i] = (uint8_t)(tostat - states);
            ret = 0;
            break;
        }
    }

    return ret;
}

void state_machine_loop(void)
{
    struct working_state* prv_stat = NULL;

    while (1) {
        if (prv_stat != cur_stat) {
            cur_stat->enter ? cur_stat->enter(cur_stat) : NULL;
            prv_stat = cur_stat;
        }
        cur_stat = cur_stat->runner(cur_stat);
        if (prv_stat != cur_stat) {
            prv_stat->exit ? prv_stat->exit(cur_stat) : NULL;
        }
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

struct working_state* trans_stat(struct working_state *from, char *to)
{
    for (int i = 0; i < MAX_SM_EDGES_NUMS && from->edges[i] != 0xff; ++i) {
        if (strncmp(states[from->edges[i]].state, to, MAX_SM_NAME_LEN) == 0) {
            return &states[from->edges[i]];
        }
    }

    return NULL;
}
/* state machine end */
