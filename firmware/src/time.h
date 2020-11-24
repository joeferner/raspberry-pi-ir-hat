#ifndef _time_h_
#define _time_h_

#include <stdint.h>

void time_setup();

uint32_t time_get();

void time_increment_tick();

#endif
