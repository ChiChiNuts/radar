/*
 * cmd_state.c
 *
 *  Created on: Oct 20, 2024
 *      Author: wormarz
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_state.h"
#include "linenoise.h"
#include "motor.h"
#include "vl6180x.h"
#include "as5600.h"
#include "elog.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

#undef LOG_TAG
#define LOG_TAG    "cmd_state"

#if !defined(UNUSED)
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

#define CMD_MOTOR_HELP "motor - control motor\n\r" \
    "Usage: motor <-s steps> <-t time span> <-d direction>\n\r" \
    "\t-s step n times\n\r" \
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
    uint32_t steps = 0;
    uint8_t direction = 0;
    int time_ms = DEFAULT_MOTOR_TIME;

    optparse_init(&options, (char**) argv);
    while ((opt = optparse(&options, "hs:d:t:")) != -1) {
        switch (opt) {
            case 's':
                steps = atoi(options.optarg);
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

    set_motor_interval(time_ms);

    if (direction == MOTOR_DIR_CW) {
        return step_clockwise(steps);
    } else if (direction == MOTOR_DIR_CCW) {
        return step_counterclockwise(steps);
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

#define CMD_RPOS_HELP "rpos - test magnetic rotary position sensor functionality\n\r" \
    "Usage: rpos [-srp]\n\r" \
    "\t-s get status of sensor\n\r" \
    "\t-r get raw rotary position\n\r" \
    "\t-p get rotary position\n\r"

static int cmd_rpos(int argc, char (*argv)[16])
{
    int opt;
    struct optparse options;

    UNUSED(argc);

    optparse_init(&options, (char**) argv);
    while ((opt = optparse(&options, "hsrpd")) != -1) {
        switch (opt) {
            case 's':
                printf("as5600 status: 0x%x\n\r", get_as5600_status());
                return 0;
            case 'r':
                printf("as5600 raw pos: %d\n\r",
                       get_as5600_angle(1) * 360 / 4096);
                return 0;
            case 'p':
                printf("as5600 pos: %d\n\r", get_as5600_angle(0) * 360 / 4096);
                return 0;
            case 'd':
                as5600_dump();
                return 0;
            case 'h':
                printf(CMD_RPOS_HELP);
                return 0;
            case '?':
                log_e("%s: %s", argv[0], options.errmsg);
                return 0;
        }
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
        } else if (!strncmp(argv[0], "rpos", sizeof("rpos") - 1)) {
            cmd_rpos(argc, argv);
        }
    }

    free_argv(argv);
}

void cmd_state_init(struct working_state *state)
{
    UNUSED(state);
    linenoiseHistorySetMaxLen(10);
}

struct working_state* cmd_state(struct working_state *self)
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
