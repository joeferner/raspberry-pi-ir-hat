#include "globals.hpp"
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
  // TODO which one?
  irRx.handleInterrupt();
}

extern "C" void DMA1_Channel2_3_IRQHandler() {
  // TODO which one?
  irRx.handleInterrupt();
}

extern "C" void DMA1_Channel4_5_6_7_IRQHandler() {
  // TODO which one?
  irRx.handleInterrupt();
}

extern "C" void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler() {
  // TODO which one?
  irRx.handleInterrupt();
}

extern "C" void TIM3_IRQHandler() {
  // TODO which one?
  irRx.handleInterrupt();
}

extern "C" void TIM16_IRQHandler() {
  irTx.handleInterrupt();
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
  currentSensor.handleInterrupt();
}
