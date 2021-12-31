#include "NVICHal.hpp"

namespace hal {

void NVICHal::setPriority(nvic::IRQnType type, uint32_t priority) { NVIC_SetPriority((IRQn_Type)type, priority); }

}  // namespace hal
