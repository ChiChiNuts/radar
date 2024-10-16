/*
 * radar_hal.c
 *
 *  Created on: Oct 7, 2024
 *      Author: wormarz
 */
#include "radar_hal.h"

/*******distance module*********/
const struct hal_param_k_v distance_params[] = {
        {.key = "unit", .val = "mm"},
        {.key = "range", .val = "100"},
};
void hal_distance_module_init(void)
{

}
void hal_distance_module_deinit(void)
{

}
int hal_distance_get_param(char *key, char *val)
{

}
uint16_t hal_distance_ranging(void)
{

}
/*******************************/

