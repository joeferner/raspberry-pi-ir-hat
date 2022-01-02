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
  const void setPrescaler(iwdg::Prescaler prescaler) const;
  const void setReloadCounter(uint32_t counter) const;
  const bool isReady() const;
  const void reloadCounter() const;
};
}  // namespace hal

#endif
