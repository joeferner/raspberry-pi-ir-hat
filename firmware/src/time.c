#include "config.h"
#include "time.h"

static volatile uint32_t _time_tick = 0;
static volatile uint32_t _time_tick_freq = 1;

void time_setup() {
    SysTick_Config(SystemCoreClock / (1000U / _time_tick_freq));
    SystemCoreClockUpdate();
}

uint32_t time_get() {
    return _time_tick;
}

void time_increment_tick() {
    _time_tick += _time_tick_freq;
}
