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

/* state machine start */
#define MAX_SM_NAME_LEN 16
#define MAX_SM_NUMS     8
#define MAX_SM_EDGES_NUMS 4

struct working_state;
typedef struct working_state* (*worker)(struct working_state *self);

struct working_state
{
    char state[MAX_SM_NAME_LEN];
    worker runner;
    void (*enter)(struct working_state*);
    void (*exit)(struct working_state*);
    union
    {
        void *args;
        struct working_state *sub;
    };
    uint8_t edges[MAX_SM_EDGES_NUMS];
};

#define CHG_STATE(_c, _ns) do { (_c) = trans_stat((_c), (_ns)); \
                                assert((_c) != NULL); \
                              } while(0)

void init_state_machine(void);
int add_state(char *name, worker wkr, void (*init)(struct working_state*));
int del_state(char *name);
int add_substate(char *parent, char *sub, worker wkr,
                 void (*init)(struct working_state*));
int del_substate(char *parent, char *sub);
int add_trans_rule(char *from, char *to);
struct working_state* trans_stat(struct working_state *from, char *to);
void setup_first_state(char *name);

void state_machine_loop(void);
/* state machine end */

#ifdef __cplusplus
}
#endif

#endif
