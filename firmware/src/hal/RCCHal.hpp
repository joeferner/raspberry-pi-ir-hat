#ifndef _RCC_HPP_
#define _RCC_HPP_

#include "config.h"

namespace hal {

namespace rcc {
enum class AHBPrescaler : uint32_t {
  DIV_1 = LL_RCC_SYSCLK_DIV_1,
  DIV_2 = LL_RCC_SYSCLK_DIV_2,
  DIV_4 = LL_RCC_SYSCLK_DIV_4,
  DIV_8 = LL_RCC_SYSCLK_DIV_8,
  DIV_16 = LL_RCC_SYSCLK_DIV_16,
  DIV_64 = LL_RCC_SYSCLK_DIV_64,
  DIV_128 = LL_RCC_SYSCLK_DIV_128,
  DIV_256 = LL_RCC_SYSCLK_DIV_256,
  DIV_512 = LL_RCC_SYSCLK_DIV_512
};

enum class SysClkSource : uint32_t {
  HSI = LL_RCC_SYS_CLKSOURCE_HSI,
  HSE = LL_RCC_SYS_CLKSOURCE_HSE,
  PLL = LL_RCC_SYS_CLKSOURCE_PLL,
  LSI = LL_RCC_SYS_CLKSOURCE_LSI,
  LSE = LL_RCC_SYS_CLKSOURCE_LSE
};

enum class APB1Prescaler : uint32_t {
  DIV_1 = LL_RCC_APB1_DIV_1,
  DIV_2 = LL_RCC_APB1_DIV_2,
  DIV_4 = LL_RCC_APB1_DIV_4,
  DIV_8 = LL_RCC_APB1_DIV_8,
  DIV_16 = LL_RCC_APB1_DIV_16
};

enum class USART1ClockSource : uint32_t {
  PCLK1 = LL_RCC_USART1_CLKSOURCE_PCLK1,
  SYSCLK = LL_RCC_USART1_CLKSOURCE_SYSCLK,
  HSI = LL_RCC_USART1_CLKSOURCE_HSI,
  LSE = LL_RCC_USART1_CLKSOURCE_LSE,
};

enum class ADCClockSource : uint32_t {
  PLL = LL_RCC_ADC_CLKSOURCE_PLL,
  SYSCLK = LL_RCC_ADC_CLKSOURCE_SYSCLK,
  HSI = LL_RCC_ADC_CLKSOURCE_HSI
};
}  // namespace rcc

class RCCHal {
 public:
  const void enableHSI() const;
  const void enableLSI() const;
  const void setAHBPrescaler(rcc::AHBPrescaler prescaler) const;
  const void setSysClkSource(rcc::SysClkSource source) const;
  const void setAPB1Prescaler(rcc::APB1Prescaler prescaler) const;
  const void setUSART1ClockSource(rcc::USART1ClockSource clockSource) const;
  const void setADCClockSource(rcc::ADCClockSource clockSource) const;
};

}  // namespace hal

#endif
