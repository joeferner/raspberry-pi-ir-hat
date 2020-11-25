#include "config.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "time.h"

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

void DMA1_Channel1_IRQHandler() {
    debug_dma_irq();
}

void DMA1_Channel2_3_IRQHandler() {
    debug_dma_irq();
}

void DMA1_Channel4_5_6_7_IRQHandler() {
    ir_rx_irq();
}

void TIM2_IRQHandler() {
    while(1);
}

void TIM3_IRQHandler() {
    ir_rx_irq();
}

void TIM6_IRQHandler() {
    while (1);
}

void TIM6_DAC_IRQHandler() {
    while (1);
}

void TIM7_IRQHandler() {
    while (1);
}

void TIM16_IRQHandler() {
    ir_tx_irq();
}

void WWDG_IRQHandler() {
    while (1);
}

void USB_IRQHandler() {
    while (1);
}

void TSC_IRQHandler() {
    while (1);
}

void USART1_IRQHandler() {
    while (1);
}

void USART2_IRQHandler() {
    while (1);
}

void USART3_4_IRQHandler() {
    while (1);
}
