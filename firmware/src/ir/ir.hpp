#ifndef _IR_REMOTE_HPP_
#define _IR_REMOTE_HPP_

#include <stdint.h>

#include "utils/Queue.hpp"

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

class DecoderState {
 public:
  DecoderState()
      : repeatCount(0), lastDecodedCommand(0), lastDecodedAddress(0), lastDecodedProtocol(Protocol::Unknown) {
  }

  void clear() {
    buffer.clear();
    repeatCount = 0;
    lastDecodedCommand = 0;
    lastDecodedAddress = 0;
    lastDecodedProtocol = Protocol::Unknown;
  }

  Queue<uint16_t, 128> buffer;
  uint32_t repeatCount;
  uint32_t lastDecodedCommand;
  uint32_t lastDecodedAddress;
  Protocol lastDecodedProtocol;
};

class DecoderResults {
 public:
  DecoderResults()
      : protocol(Protocol::Unknown),
        numberOfBits(0),
        command(0),
        address(0),
        endian(Endian::MSB),
        autoRepeat(false),
        parityFailed(false),
        repeat(false) {
  }

  Protocol protocol;
  uint32_t numberOfBits;
  uint32_t command;
  uint32_t address;
  Endian endian;
  bool autoRepeat;
  bool parityFailed;
  bool repeat;
};

}  // namespace ir

#endif
