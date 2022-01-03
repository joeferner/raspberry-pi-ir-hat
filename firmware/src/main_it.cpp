#include "current_sensor.hpp"
#include "globals.hpp"
#include "ir_rx.hpp"
#include "ir_tx.hpp"
#include "main.h"

extern "C" void NMI_Handler() {
}

extern "C" void HardFault_Handler() {
  while (1)
    ;
}

extern "C" void SVC_Handler() {
}

extern "C" void PendSV_Handler() {
}

extern "C" void SysTick_Handler() {
  clocks.incrementTick();
}

extern "C" void DMA1_Channel1_IRQHandler() {
  ir_rx_irq();
}

extern "C" void DMA1_Channel2_3_IRQHandler() {
  ir_rx_irq();
}

extern "C" void DMA1_Channel4_5_6_7_IRQHandler() {
  ir_rx_irq();
}

extern "C" void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler() {
  ir_rx_irq();
}

extern "C" void TIM3_IRQHandler() {
  ir_rx_irq();
}

extern "C" void TIM16_IRQHandler() {
  ir_tx_irq();
}

extern "C" void WWDG_IRQHandler() {
  while (1)
    ;
}

extern "C" void USART1_IRQHandler() {
  debugUsart.handleInterrupt();
}

extern "C" void USART2_IRQHandler() {
  rpiUsart.handleInterrupt();
}

extern "C" void TIM2_IRQHandler() {
  while (1)
    ;
}

extern "C" void TIM1_CC_IRQHandler() {
  while (1)
    ;
}

extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler() {
  while (1)
    ;
}

extern "C" void ADC1_IRQHandler(void) {
  // TODO
  if (LL_ADC_IsActiveFlag_EOC(ADC1)) {
    LL_ADC_ClearFlag_EOC(ADC1);
    current_sensor_end_of_conversion();
  }

  if (LL_ADC_IsActiveFlag_EOS(ADC1)) {
    LL_ADC_ClearFlag_EOS(ADC1);
    current_sensor_end_of_sequence();
  }

  if (LL_ADC_IsActiveFlag_OVR(ADC1)) {
    LL_ADC_ClearFlag_OVR(ADC1);
    current_sensor_overrun_error();
  }
}
