#ifndef _BUS_HPP_
#define _BUS_HPP_

#include "main.h"

namespace hal {

class Bus {
 public:
  void enableSyscfgClock() const;
  void enablePwrClock() const;
};

}  // namespace hal

#endif
