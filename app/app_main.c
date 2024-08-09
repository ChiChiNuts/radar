#include "misc.h"
#include "elog.h"
#include "vl6180x.h"
#include "gpio.h"
#include "motor.h"
#include "usart.h"
#include "stdio.h"
#include "linenoise.h"

#undef LOG_TAG
#define LOG_TAG    "app_main"

#ifdef COMMIT_ID
#define COMMIT STR2(COMMIT_ID)
#else
#define COMMIT ""
#endif

static struct working_state* scan_state(struct working_state *self)
{
    log_i("into %s", self->state);
    delay_ms(1000);
    CHG_STATE(self, "idle");

    return self;
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

static struct working_state* cmd_state(struct working_state *self)
{
    char *line = NULL;
    log_i("into %s", self->state);
//    delay_ms(1000);
    while((line = linenoise(APPNAME"> ")) != NULL) {
//        printf("You wrote: %s\n", line);
        linenoiseFree(line); /* Or just free(line) if you use libc malloc. */
    }
    CHG_STATE(self, "idle");

    return self;
}

int app_main(void)
{
    log_i("radar start!");
    log_i("commit[%s]", COMMIT);

    init_state_machine();
    add_state("idle", idle_state, NULL);
    add_state("CMD", cmd_state, NULL);
    add_state("scan", scan_state, NULL);

    state_machine_loop();

    while (1) {
    };
    return 0;
}
