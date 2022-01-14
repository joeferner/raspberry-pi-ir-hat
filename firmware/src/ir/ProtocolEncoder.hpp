#ifndef _IR_PROTOCOL_ENCODER_HPP_
#define _IR_PROTOCOL_ENCODER_HPP_

#include <stddef.h>
#include <stdint.h>

#include "test/test.hpp"
#include "ir.hpp"
#include "peripheral/IrTx.hpp"

namespace ir {

enum class StopBit
{
  True,
  False
};

class ProtocolEncoder {
public:
  virtual bool encode(
    peripheral::IrTx& irTx,
    Protocol protocol,
    uint32_t aCommand,
    uint32_t aAddress,
    uint32_t aNumberOfRepeats) const = 0;

protected:
  void sendPulseDistanceWidthData(
    peripheral::IrTx& irTx,
    unsigned int aOneMarkMicros,
    unsigned int aOneSpaceMicros,
    unsigned int aZeroMarkMicros,
    unsigned int aZeroSpaceMicros,
    uint32_t aData,
    uint8_t aNumberOfBits,
    Endian aMSBfirst,
    StopBit aSendStopBit) const;
};

}  // namespace ir

#endif
