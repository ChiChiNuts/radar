#include "misc.h"
#include "elog.h"
#include "vl6180x.h"
#include "gpio.h"
#include "motor.h"
#include "usart.h"
#include "stdio.h"
#include "linenoise.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

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

static void idle_enter(struct working_state *state)
{
    log_i("enter %s", __func__);
}

static void idle_exit(struct working_state *state)
{
    log_i("exit %s", __func__);
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

/***********************CMD start******************************/
#define CMD_MOTOR_HELP "motor - control motor\n\r" \
    "Usage: motor <-s/p> [-t time span] [-d direction]\n\r" \
    "\t-s start to run motor\n\r" \
	"\t-p pause to run motor\n\r" \
	"\t-t set motor running time span\n\r" \
	"\t-d set motor running direction\n\r" \
	"\t-h show help information.\n\r"

#define MOTOR_DIR_CW   1
#define MOTOR_DIR_CCW  2
#define DEFAULT_MOTOR_TIME 20
static int cmd_motor(int argc, char (*argv)[16])
{
    int opt;
    struct optparse options;
    bool start = false, stop = false;
    uint8_t direction = 0;
    int time_ms = DEFAULT_MOTOR_TIME;

    UNUSED(argc);

    optparse_init(&options, (char**) argv);
    while ((opt = optparse(&options, "hspd:t:")) != -1) {
        switch (opt) {
            case 's':
                start = true;
                stop = false;
                break;
            case 'p':
                start = false;
                stop = true;
                break;
            case 'd':
                if (!strncmp(options.optarg, "cw", sizeof("cw") - 1)) {
                    direction = MOTOR_DIR_CW;
                } else if (!strncmp(options.optarg, "ccw", sizeof("ccw") - 1)) {
                    direction = MOTOR_DIR_CCW;
                }
                break;
            case 't':
                time_ms = atoi(options.optarg);
                break;
            case 'h':
                printf(CMD_MOTOR_HELP);
                return 0;
            case '?':
                log_e("%s: %s", argv[0], options.errmsg);
                return 0;
        }
    }

    if (start) {
        if (direction == MOTOR_DIR_CW) {
            step_clockwise(time_ms);
        } else if (direction == MOTOR_DIR_CCW) {
            step_counterclockwise(time_ms);
        }
    } else if (stop) {
        step_motor_stop();
    }

    return 0;
}

#define CMD_TOF_HELP "tof - test ToF functionality\n\r" \
    "Usage: tof [-i] [-r]\n\r" \
    "\t-i Initialize ToF\n\r" \
    "\t-r get distance from ToF\n\r"

static int cmd_tof(int argc, char (*argv)[16])
{
    int opt;
    struct optparse options;
    static bool is_init = false;
    bool do_read = false, do_init = false;
    uint8_t range = 0;

    UNUSED(argc);

    optparse_init(&options, (char**) argv);
    while ((opt = optparse(&options, "hir")) != -1) {
        switch (opt) {
            case 'i':
                do_init = true;
                break;
            case 'r':
                do_read = true;
                break;
            case 'h':
                printf(CMD_TOF_HELP);
                return 0;
            case '?':
                log_e("%s: %s", argv[0], options.errmsg);
                return 0;
        }
    }

    if (!is_init && do_init) {
        is_init = !VL6180X_Init();
    }

    if (is_init && do_read) {
        VL6180x_Ranging();
        delay_ms(20);
        range = VL6180_Read_Range();
        log_i("range : %d", range);
    }

    return 0;
}

static void cli_handler(char *cmd)
{
    char (*argv)[16];
    int argc;

    cmd2argv(cmd, (void**) &argv, &argc);

    if (argc >= 1) {
        if (!strncmp(argv[0], "motor", sizeof("motor") - 1)) {
            cmd_motor(argc, argv);
        } else if (!strncmp(argv[0], "tof", sizeof("tof") - 1)) {
            cmd_tof(argc, argv);
        }
    }

    free_argv(argv);
}

static void cmd_state_init(struct working_state *state)
{
    UNUSED(state);
    linenoiseHistorySetMaxLen(10);
}

static struct working_state* cmd_state(struct working_state *self)
{
    char *line = NULL;
    log_i("into %s", self->state);

    while ((line = linenoise(APPNAME"> ")) != NULL) {
        cli_handler(line);
        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
        }
        linenoiseFree(line);
    }
    CHG_STATE(self, "idle");

    return self;
}

/************************CMD end*****************************/

int app_main(void)
{
    log_i("radar start!");
    log_i("commit[%s]", COMMIT);

    init_state_machine();
    assert(add_state("idle", idle_state, idle_state_init) == 0);
    assert(add_state("CMD", cmd_state, cmd_state_init) == 0);
    assert(add_state("scan", scan_state, NULL) == 0);

    assert(add_trans_rule("idle", "CMD") == 0);
    assert(add_trans_rule("idle", "scan") == 0);
    assert(add_trans_rule("CMD", "idle") == 0);
    assert(add_trans_rule("scan", "idle") == 0);

    state_machine_loop();

    return 0;
}
