#include "time.h"

#include "config.h"

static volatile uint32_t time_tick = 0;
static volatile uint32_t time_tick_freq = 1;

void time_setup() {
  SysTick_Config(SystemCoreClock / (1000U / time_tick_freq));
  SystemCoreClockUpdate();
}

uint32_t time_get() { return time_tick; }

void time_increment_tick() { time_tick += time_tick_freq; }
