
#include <stm32f0xx_ll_bus.h>
#include <stm32f0xx_ll_system.h>
#include <stm32f0xx_ll_rcc.h>
#include <stm32f0xx_ll_utils.h>
#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"

void setup();

void setup_system_clock();

void loop();

int main() {
    setup();
    while (1) {
        loop();
    }
}

void setup() {
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    setup_system_clock();

    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
    NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

    debug_setup();
    ir_rx_setup();
    ir_tx_setup();
    debug_send_string("READY\n");
}

void setup_system_clock() {
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {
    }
    LL_RCC_HSI48_Enable();

    // Wait till HSI48 is ready
    while (LL_RCC_HSI48_IsReady() != 1) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI48);

    // Wait till System clock is ready
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI48) {
    }
    LL_Init1msTick(48000000);
    LL_SetSystemCoreClock(48000000);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);
}

void loop() {
    debug_loop();
    ir_rx_loop();
}

void Error_Handler(void) {
    while (1);
}

void debug_rx(const uint8_t *data, size_t data_len) {
    debug_send_string("OK\n");
    debug_tx(data, data_len);
    debug_send_string("\n");
}

void ir_rx_received(ir_rx_value value) {
    debug_send_string("P");
    debug_send_uint32(value);
    debug_send_string("\n");
}