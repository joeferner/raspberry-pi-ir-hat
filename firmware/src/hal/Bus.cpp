#include "Bus.hpp"

namespace hal {

const void Bus::enableSyscfgClock() const {
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
}
const void Bus::enablePwrClock() const {
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
}

}  // namespace hal