#include "RCC.hpp"

namespace hal {

void RCCHal::enableHSI() const {
  LL_RCC_HSI_Enable();
  while (LL_RCC_HSI_IsReady() != 1) {
  }
}

void RCCHal::enableLSI() const {
  LL_RCC_LSI_Enable();
  while (LL_RCC_LSI_IsReady() != 1) {
  }
}

void RCCHal::setAHBPrescaler(rcc::AHBPrescaler prescaler) const {
  LL_RCC_SetAHBPrescaler((uint32_t)prescaler);
}

void RCCHal::setSysClkSource(rcc::SysClkSource source) const {
  LL_RCC_SetSysClkSource((uint32_t)source);
  while (LL_RCC_GetSysClkSource() != (uint32_t)source) {
  }
}

void RCCHal::setAPB1Prescaler(rcc::APB1Prescaler prescaler) const {
  LL_RCC_SetAPB1Prescaler((uint32_t)prescaler);
}

void RCCHal::setUSART1ClockSource(rcc::USART1ClockSource clockSource) const {
  LL_RCC_SetUSARTClockSource((uint32_t)clockSource);
}

void RCCHal::setADCClockSource(rcc::ADCClockSource clockSource) const {
  LL_RCC_SetADCClockSource((uint32_t)clockSource);
}

const uint32_t RCCHal::getUSART1ClockFrequency() const {
  return LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE);
}

const uint32_t RCCHal::getPCLK1Frequency() const {
  LL_RCC_ClocksTypeDef clocks;
  LL_RCC_GetSystemClocksFreq(&clocks);
  return clocks.PCLK1_Frequency;
}

}  // namespace hal