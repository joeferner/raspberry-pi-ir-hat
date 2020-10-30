#include "time.h"

static volatile uint32_t _time_tick = 0;
static volatile uint32_t _time_tick_freq = 1;

uint32_t time_get() {
    return _time_tick;
}

void time_increment_tick() {
    _time_tick += _time_tick_freq;
}
