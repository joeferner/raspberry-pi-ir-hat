#ifndef _NVIC_HPP_
#define _NVIC_HPP_

#include "config.h"

namespace hal {

namespace nvic {
enum class IRQnType : int
{
  NonMaskableInt_Irq = NonMaskableInt_IRQn,
  HardFault_Irq = HardFault_IRQn,
  SVC_Irq = SVC_IRQn,
  PendSV_Irq = PendSV_IRQn,
  SysTick_Irq = SysTick_IRQn,
  WWDG_Irq = WWDG_IRQn,
  PVD_Irq = PVD_IRQn,
  RTC_TAMP_Irq = RTC_TAMP_IRQn,
  FLASH_Irq = FLASH_IRQn,
  RCC_Irq = RCC_IRQn,
  EXTI0_1_Irq = EXTI0_1_IRQn,
  EXTI2_3_Irq = EXTI2_3_IRQn,
  EXTI4_15_Irq = EXTI4_15_IRQn,
  DMA1_Channel1_Irq = DMA1_Channel1_IRQn,
  DMA1_Channel2_3_Irq = DMA1_Channel2_3_IRQn,
  DMA1_Ch4_5_DMAMUX1_OVR_Irq = DMA1_Ch4_5_DMAMUX1_OVR_IRQn,
  ADC1_Irq = ADC1_IRQn,
  TIM1_BRK_UP_TRG_COM_Irq = TIM1_BRK_UP_TRG_COM_IRQn,
  TIM1_CC_Irq = TIM1_CC_IRQn,
  TIM2_Irq = TIM2_IRQn,
  TIM3_Irq = TIM3_IRQn,
  LPTIM1_Irq = LPTIM1_IRQn,
  LPTIM2_Irq = LPTIM2_IRQn,
  TIM14_Irq = TIM14_IRQn,
  TIM16_Irq = TIM16_IRQn,
  TIM17_Irq = TIM17_IRQn,
  I2C1_Irq = I2C1_IRQn,
  I2C2_Irq = I2C2_IRQn,
  SPI1_Irq = SPI1_IRQn,
  SPI2_Irq = SPI2_IRQn,
  USART1_Irq = USART1_IRQn,
  USART2_Irq = USART2_IRQn,
  LPUART1_Irq = LPUART1_IRQn
};
}  // namespace nvic

class NVICHal {
 public:
  void setPriority(nvic::IRQnType type, uint32_t priority);
  void enableInterrupt(nvic::IRQnType type);
};

}  // namespace hal

#endif
