#include "config.h"
#include "current_sensor.h"
#include "debug.h"
#include "ir_rx.h"
#include "ir_tx.h"
#include "rpi.h"
#include "time.h"

void NMI_Handler() {
}

void HardFault_Handler() {
  while (1)
    ;
}

void SVC_Handler() {
}

void PendSV_Handler() {
}

void SysTick_Handler() {
  time_increment_tick();
}

void DMA1_Channel1_IRQHandler() {
  ir_rx_irq();
}

void DMA1_Channel2_3_IRQHandler() {
  ir_rx_irq();
}

void DMA1_Channel4_5_6_7_IRQHandler() {
  ir_rx_irq();
}

void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler() {
  ir_rx_irq();
}

void TIM3_IRQHandler() {
  ir_rx_irq();
}

void TIM16_IRQHandler() {
  ir_tx_irq();
}

void WWDG_IRQHandler() {
  while (1)
    ;
}

void USART1_IRQHandler() {
  debug_usart_irq();
}

void USART2_IRQHandler() {
  rpi_usart_irq();
}

void TIM2_IRQHandler() {
  while (1)
    ;
}

void TIM1_CC_IRQHandler() {
  while (1)
    ;
}

void TIM1_BRK_UP_TRG_COM_IRQHandler() {
  while (1)
    ;
}

void ADC1_IRQHandler(void) {
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
