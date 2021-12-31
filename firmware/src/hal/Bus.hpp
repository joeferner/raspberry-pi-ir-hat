#ifndef _BUS_HPP_
#define _BUS_HPP_

#include "config.h"

namespace hal {

class Bus {
 public:
  const void enableSyscfgClock() const;
  const void enablePwrClock() const;
};

}  // namespace hal

#endif
