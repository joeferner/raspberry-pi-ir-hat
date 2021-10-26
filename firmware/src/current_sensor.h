#ifndef _CURRENT_SENSOR_H_
#define _CURRENT_SENSOR_H_

#include <stdint.h>

void current_sensor_setup();
void current_sensor_loop();
void current_sensor_end_of_conversion();
void current_sensor_end_of_sequence();
void current_sensor_overrun_error();

uint16_t current_sensor_get_ref();
uint16_t current_sensor_get1();
uint16_t current_sensor_get2();

#endif
