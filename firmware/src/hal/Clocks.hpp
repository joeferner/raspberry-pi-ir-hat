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

  void init1msTick(uint32_t hclkFrequency) {
    LL_Init1msTick(hclkFrequency);
    SystemCoreClockUpdate();
  }

  const uint32_t getTickCount() const {
    return this->tickCount;
  }

  void incrementTick() {
    this->tickCount++;
  }

  void setSystemCoreClock(uint32_t hclkFrequency) {
    LL_SetSystemCoreClock(hclkFrequency);
  }

  void enableGPIOAClock() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  }

  void enableGPIOBClock() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  }

  void enableGPIOFClock() {
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
  }

  void enableDMA1Clock() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  }

  void enableUSART1Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  }

  void enableUSART2Clock() {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
  }

  void enableTIM1Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
  }

  void enableTIM2Clock() {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
  }

  void enableTIM3Clock() {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
  }

  void enableTIM14Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
  }

  void enableTIM16Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
  }

  void enableTIM17Clock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
  }
};
}  // namespace hal

#endif
