#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

#include "config.h"

namespace hal {

namespace system {
enum class IRModulationEnvelopeSignalSource : uint32_t {
  IR_TIM16 = LL_SYSCFG_IR_MOD_TIM16,
  IR_USART1 = LL_SYSCFG_IR_MOD_USART1,
#if defined(LL_SYSCFG_IR_MOD_USART4)
  IR_USART4 = LL_SYSCFG_IR_MOD_USART4
#else
  IR_USART2 = LL_SYSCFG_IR_MOD_USART2
#endif
};

enum class IRPolarity : uint32_t { Inverted = LL_SYSCFG_IR_POL_INVERTED, NotInverted = LL_SYSCFG_IR_POL_NOT_INVERTED };
}  // namespace system

class System {
 public:
  const void setIRModulationEnvelopeSignalSource(system::IRModulationEnvelopeSignalSource source) const;
  const void setIRPolarity(system::IRPolarity polarity);
};
}  // namespace hal

#endif
