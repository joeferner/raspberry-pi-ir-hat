#include "System.hpp"

namespace hal {
const void System::setIRModulationEnvelopeSignalSource(system::IRModulationEnvelopeSignalSource source) const {
  LL_SYSCFG_SetIRModEnvelopeSignal((uint32_t)source);
}

const void System::setIRPolarity(system::IRPolarity polarity) {
  LL_SYSCFG_SetIRPolarity((uint32_t)polarity);
}

}  // namespace hal