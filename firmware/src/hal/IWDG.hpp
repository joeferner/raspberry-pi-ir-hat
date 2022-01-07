#ifndef _IWDG_HPP_
#define _IWDG_HPP_

#include "main.h"

namespace hal {
namespace iwdg {
enum class Prescaler
{
  Prescaler4 = LL_IWDG_PRESCALER_4,
  Prescaler8 = LL_IWDG_PRESCALER_8,
  Prescaler16 = LL_IWDG_PRESCALER_16,
  Prescaler32 = LL_IWDG_PRESCALER_32,
  Prescaler64 = LL_IWDG_PRESCALER_64,
  Prescaler128 = LL_IWDG_PRESCALER_128,
  Prescaler256 = LL_IWDG_PRESCALER_256,
};
}  // namespace iwdg

class IWDGHal {
 public:
  void setPrescaler(iwdg::Prescaler prescaler) const {
    LL_IWDG_SetPrescaler(IWDG, (uint32_t)prescaler);
  }

  void setReloadCounter(uint32_t counter) const {
    assert_param(counter < 0x0fff);
    LL_IWDG_SetReloadCounter(IWDG, counter);
  }

  const bool isReady() const {
    return LL_IWDG_IsReady(IWDG) == 1;
  }

  void reloadCounter() const {
    LL_IWDG_ReloadCounter(IWDG);
  }

  void enable() {
    LL_IWDG_Enable(IWDG);
  }
};
}  // namespace hal

#endif
