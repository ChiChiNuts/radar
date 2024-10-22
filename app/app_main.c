#include "misc.h"
#include "elog.h"
#include "gpio.h"
#include "usart.h"
#include <stdbool.h>
#include "cmd_state.h"
#include "scan_state.h"

#undef LOG_TAG
#define LOG_TAG    "app_main"

#ifdef COMMIT_ID
#define COMMIT STR2(COMMIT_ID)
#else
#define COMMIT ""
#endif


static void idle_enter(struct working_state *state)
{
    log_i("%s", __func__);
}

static void idle_exit(struct working_state *state)
{
    log_i("%s", __func__);
}

static void idle_state_init(struct working_state *state)
{
    state->enter = idle_enter;
    state->exit = idle_exit;
}

static struct working_state* idle_state(struct working_state *self)
{
    char ch;
    if (HAL_UART_Receive(&huart1, (uint8_t*) &ch, 1, 1000) == HAL_OK) {
        if (ch == 'C') {
            CHG_STATE(self, "CMD");
        } else if (ch == 'S') {
            CHG_STATE(self, "scan");
        }
    }
    return self;
}


int app_main(void)
{
    log_i("radar start!");
    log_i("commit[%s]", COMMIT);

    init_state_machine();
    assert(add_state("idle", idle_state, idle_state_init) == 0);
    assert(add_state("CMD", cmd_state, cmd_state_init) == 0);
    assert(add_state("scan", NULL, NULL) == 0);
    assert(add_substate("scan", "sub1", sub1_state, NULL) == 0);
    assert(add_state("sub2", scan_state, NULL) == 0);

    assert(add_trans_rule("idle", "CMD") == 0);
    assert(add_trans_rule("idle", "scan") == 0);
    assert(add_trans_rule("CMD", "idle") == 0);
    assert(add_trans_rule("scan", "idle") == 0);
    assert(add_trans_rule("sub1", "sub2") == 0);
    assert(add_trans_rule("sub2", "idle") == 0);

    state_machine_loop();

    return 0;
}
