#include "RCCHal.hpp"

namespace hal {

const void RCCHal::enableHSI() const {
  LL_RCC_HSI_Enable();
  while (LL_RCC_HSI_IsReady() != 1) {
  }
}

const void RCCHal::enableLSI() const {
  LL_RCC_LSI_Enable();
  while (LL_RCC_LSI_IsReady() != 1) {
  }
}

const void RCCHal::setAHBPrescaler(rcc::AHBPrescaler prescaler) const { LL_RCC_SetAHBPrescaler((uint32_t)prescaler); }

const void RCCHal::setSysClkSource(rcc::SysClkSource source) const {
  LL_RCC_SetSysClkSource((uint32_t)source);
  while (LL_RCC_GetSysClkSource() != (uint32_t)source) {
  }
}

const void RCCHal::setAPB1Prescaler(rcc::APB1Prescaler prescaler) const {
  LL_RCC_SetAPB1Prescaler((uint32_t)prescaler);
}

const void RCCHal::setUSART1ClockSource(rcc::USART1ClockSource clockSource) const {
  LL_RCC_SetUSARTClockSource((uint32_t)clockSource);
}

const void RCCHal::setADCClockSource(rcc::ADCClockSource clockSource) const {
  LL_RCC_SetADCClockSource((uint32_t)clockSource);
}

}  // namespace hal