
#include <stm32f0xx_ll_bus.h>
#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"

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

    time_setup();

    NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
    NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 0);
    NVIC_EnableIRQ(SysTick_IRQn);

    debug_setup();
    ir_rx_setup();
    ir_tx_setup();
    debug_send_string("READY\n");
}

void loop() {
    debug_loop();
    ir_rx_loop();
}

void Error_Handler(void) {
    while (1);
}

void debug_rx(const uint8_t *data, size_t data_len) {
    debug_send_string("OK ");
    debug_tx(data, data_len);
    debug_send_string("\n");
}

void ir_rx_received(ir_rx_value value) {
    debug_send_string("P");
    debug_send_uint32(value);
    debug_send_string("\n");
}