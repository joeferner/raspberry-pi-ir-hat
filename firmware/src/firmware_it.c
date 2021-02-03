#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"
#include "rpi.h"

void NMI_Handler() {
}

void HardFault_Handler() {
    while (1);
}

void SVC_Handler() {
}

void PendSV_Handler() {
}

void SysTick_Handler() {
    time_increment_tick();
}

void DMA1_Channel1_IRQHandler() {
    debug_dma_irq();
    ir_rx_irq();
    rpi_dma_irq();
}

void DMA1_Channel2_3_IRQHandler() {
    debug_dma_irq();
    ir_rx_irq();
    rpi_dma_irq();
}

void DMA1_Channel4_5_6_7_IRQHandler() {
    debug_dma_irq();
    ir_rx_irq();
    rpi_dma_irq();
}

void TIM3_IRQHandler() {
    ir_rx_irq();
}

void TIM16_IRQHandler() {
    ir_tx_irq();
}
