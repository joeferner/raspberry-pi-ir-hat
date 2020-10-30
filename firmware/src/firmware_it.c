#include "config.h"
#include "debug.h"

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
    while (1) {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
    // TODO HAL_IncTick();
}

/**
 * @brief This function handles DMA1 channel 2 and 3 interrupts.
 */
void DMA1_Channel2_3_IRQHandler(void) {
    debug_dma_irq();
}

/**
 * @brief This function handles DMA1 channel 4, 5, 6 and 7 interrupts.
 */
void DMA1_Channel4_5_6_7_IRQHandler(void) {
    // TODO HAL_DMA_IRQHandler(&hdma_tim2_ch1);
}

/**
 * @brief This function handles TIM2 global interrupt.
 */
void TIM2_IRQHandler(void) {
    // TODO HAL_TIM_IRQHandler(&htim2);
}
