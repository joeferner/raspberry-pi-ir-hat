#ifndef _CLOCKS_HPP_
#define _CLOCKS_HPP_

#include "main.h"

namespace hal {
class Clocks {
private:
  volatile uint32_t tickCount;

public:
  Clocks() : tickCount(0) {
  }

  const void init1msTick(uint32_t hclkFrequency) {
    LL_Init1msTick(hclkFrequency);
    SystemCoreClockUpdate();
  }

  const uint32_t getTickCount() const {
    return this->tickCount;
  }

  const void incrementTick() {
    this->tickCount++;
  }

  const void setSystemCoreClock(uint32_t hclkFrequency) {
    LL_SetSystemCoreClock(hclkFrequency);
  }

  const void enableGPIOAClock() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  }

  const void enableGPIOBClock() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  }

  const void enableGPIOFClock() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
  }

  const void enableDMA1Clock() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  }

  const void enableUSART1Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  }

  const void enableUSART2Clock() {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
  }

  const void enableTIM1Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
  }

  const void enableTIM2Clock() {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
  }

  const void enableTIM3Clock() {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
  }

  const void enableTIM14Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
  }

  const void enableTIM16Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
  }

  const void enableTIM17Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
  }
};
}  // namespace hal

#endif
