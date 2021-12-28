#ifndef TIME_H
#define TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void time_setup();

uint32_t time_get();

void time_increment_tick();

#ifdef __cplusplus
}
#endif

#endif
