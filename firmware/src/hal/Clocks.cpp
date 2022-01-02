#include "Clocks.hpp"

namespace hal {
const void Clocks::init1msTick(uint32_t hclkFrequency) const { LL_Init1msTick(hclkFrequency); }

const void Clocks::setSystemCoreClock(uint32_t hclkFrequency) const { LL_SetSystemCoreClock(hclkFrequency); }

const void Clocks::enableGPIOAClock() const { LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); }
const void Clocks::enableGPIOBClock() const { LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); }
const void Clocks::enableGPIOFClock() const { LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF); }
const void Clocks::enableDMA1Clock() const { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1); }
const void Clocks::enableUSART1Clock() const { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1); }
const void Clocks::enableUSART2Clock() const { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2); }
const void Clocks::enableTIM3Clock() const { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); }

}  // namespace hal
