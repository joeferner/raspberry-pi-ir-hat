#include "IWDG.hpp"

namespace hal {
const void IWDGHal::setPrescaler(iwdg::Prescaler prescaler) const {
  LL_IWDG_SetPrescaler(IWDG, (uint32_t)prescaler);
}

const void IWDGHal::setReloadCounter(uint32_t counter) const {
  assert_param(counter < 0x0fff);
  LL_IWDG_SetReloadCounter(IWDG, counter);
}

const bool IWDGHal::isReady() const {
  return LL_IWDG_IsReady(IWDG) == 1;
}

const void IWDGHal::reloadCounter() const {
  LL_IWDG_ReloadCounter(IWDG);
}
}  // namespace hal