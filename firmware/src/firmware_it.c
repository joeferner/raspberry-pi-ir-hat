#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "time.h"
#include "ir_tx.h"

void NMI_Handler() {
}

void HardFault_Handler() {
    while (1) {
    }
}

void SVC_Handler() {
}

void PendSV_Handler() {
}

void SysTick_Handler() {
    time_increment_tick();
}

void DMA1_Channel2_3_IRQHandler() {
    debug_dma_irq();
}

void DMA1_Channel4_5_6_7_IRQHandler() {
    ir_rx_irq();
}

void TIM2_IRQHandler() {
    ir_rx_irq();
}

void TIM16_IRQHandler() {
    ir_tx_irq();
}