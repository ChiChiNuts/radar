#include "misc.h"
#include "elog.h"
#include "vl6180x.h"
#include "gpio.h"
#include "motor.h"

#undef LOG_TAG
#define LOG_TAG    "app_main"

#ifdef COMMIT_ID
#define COMMIT STR2(COMMIT_ID)
#else
#define COMMIT ""
#endif

int app_main(void)
{
	uint8_t range = 0;
	bool clockwise = true;

	log_i("radar start!");
	log_i("commit[%s]", COMMIT);
	step_motor_init();
//	VL6180X_Init();
	step_clockwise(1);

	while(1) {
     delay_ms(50);
//     VL6180x_Ranging();
//     range = VL6180_Read_Range();
//	   log_i("range : %d", range);

		if (++range >= 200) {
			step_motor_stop();
			log_i("reverse");
			clockwise = !clockwise;
			range = 0;
			if(clockwise) {
			  step_clockwise(1);
			} else {
			  step_counterclockwise(1);
			}
		}
	}
	return 0;
}
