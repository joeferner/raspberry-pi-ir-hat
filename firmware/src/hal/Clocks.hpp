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

  void delayMicros(uint32_t microseconds) {
    // TODO how do we delay microseconds?
    delayMs(microseconds / 1000);
  }

  void delayMs(uint32_t milliseconds) {
    // TODO this assumes 1ms tick
    uint32_t start = this->tickCount;
    while ((this->tickCount - start) < milliseconds);
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

  void enableADCClock() {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
  }
};
}  // namespace hal

#endif
