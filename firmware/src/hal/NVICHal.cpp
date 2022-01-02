#include "NVICHal.hpp"

namespace hal {

void NVICHal::setPriority(nvic::IRQnType type, uint32_t priority) { NVIC_SetPriority((IRQn_Type)type, priority); }

void NVICHal::enableInterrupt(nvic::IRQnType type) { NVIC_EnableIRQ((IRQn_Type)type); }

}  // namespace hal
