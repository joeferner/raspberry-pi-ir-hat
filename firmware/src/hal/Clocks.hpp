#ifndef _CLOCKS_HPP_
#define _CLOCKS_HPP_

#include "config.h"

namespace hal {
class Clocks {
 public:
  const void init1msTick(uint32_t hclkFrequency) const;
  const void setSystemCoreClock(uint32_t hclkFrequency) const;
  const void enableGPIOAClock() const;
  const void enableGPIOBClock() const;
  const void enableGPIOFClock() const;
  const void enableDMA1Clock() const;
  const void enableUSART1Clock() const;
};
}  // namespace hal

#endif
