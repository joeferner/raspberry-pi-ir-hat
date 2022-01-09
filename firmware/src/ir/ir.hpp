#ifndef _IR_REMOTE_HPP_
#define _IR_REMOTE_HPP_

#include <stdint.h>

namespace ir {

enum class Protocol : uint8_t
{
  Unknown = 0,
  Denon = 1,
  Sharp = 2,
  Apple = 3,
  NEC = 4,
  Onkyo = 5
};

enum class Endian
{
  MSB,
  LSB
};

}  // namespace ir

#endif
