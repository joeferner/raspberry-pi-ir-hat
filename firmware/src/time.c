#include <stm32f0xx_ll_system.h>
#include <stm32f0xx_ll_rcc.h>
#include <stm32f0xx_ll_utils.h>
#include "time.h"

static volatile uint32_t _time_tick = 0;
static volatile uint32_t _time_tick_freq = 1;

void time_setup() {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0) {
    }
    LL_RCC_HSI_Enable();

    // Wait till HSI is ready
    while (LL_RCC_HSI_IsReady() != 1) {
    }
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

    // Wait till System clock is ready
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {
    }
    LL_Init1msTick(8000000);
    LL_SetSystemCoreClock(8000000);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);

    SysTick_Config(SystemCoreClock / (1000U / _time_tick_freq));

    SystemCoreClockUpdate();
}

uint32_t time_get() {
    return _time_tick;
}

void time_increment_tick() {
    _time_tick += _time_tick_freq;
}
