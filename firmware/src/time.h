#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void time_setup();

uint32_t time_get();

void time_increment_tick();

#endif
