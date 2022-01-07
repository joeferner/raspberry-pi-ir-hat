#include "System.hpp"

namespace hal {
void System::setIRModulationEnvelopeSignalSource(system::IRModulationEnvelopeSignalSource source) const {
  LL_SYSCFG_SetIRModEnvelopeSignal((uint32_t)source);
}

void System::setIRPolarity(system::IRPolarity polarity) {
  LL_SYSCFG_SetIRPolarity((uint32_t)polarity);
}

}  // namespace hal