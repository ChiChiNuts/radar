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
#define MAX_SM_NUMS     4

struct working_state;
typedef struct working_state* (*worker)(struct working_state* self, void* args);

struct working_state {
  char state[MAX_SM_NAME_LEN];
  worker runner;
};

#define CHG_STATE(_c, _ns) do { (_c) = stat_lookup((_ns)); } while(0)

void init_state_machine(void);
int add_state(char *name, worker wkr);
struct working_state* stat_lookup(char *name);

void state_machine_loop(void);
/* state machine end */

#ifdef __cplusplus
}
#endif

#endif
