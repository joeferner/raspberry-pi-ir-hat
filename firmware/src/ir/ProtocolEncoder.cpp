#include "ProtocolEncoder.hpp"

namespace ir {
void ProtocolEncoder::sendPulseDistanceWidthData(
    peripheral::IrTx& irTx,
    unsigned int aOneMarkMicros,
    unsigned int aOneSpaceMicros,
    unsigned int aZeroMarkMicros,
    unsigned int aZeroSpaceMicros,
    uint32_t aData,
    uint8_t aNumberOfBits,
    Endian aMSBfirst,
    StopBit aSendStopBit) const {
  if (aMSBfirst == Endian::MSB) {  // Send the MSB first.
                                   // send data from MSB to LSB until mask bit is shifted out
    for (uint32_t tMask = 1UL << (aNumberOfBits - 1); tMask; tMask >>= 1) {
      if (aData & tMask) {
        irTx.write(aOneMarkMicros, aOneSpaceMicros);
      } else {
        irTx.write(aZeroMarkMicros, aZeroSpaceMicros);
      }
    }
  } else {  // Send the Least Significant Bit (LSB) first / MSB last.
    for (uint_fast8_t bit = 0; bit < aNumberOfBits; bit++, aData >>= 1)
      if (aData & 1) {  // Send a 1
        irTx.write(aOneMarkMicros, aOneSpaceMicros);
      } else {  // Send a 0
        irTx.write(aZeroMarkMicros, aZeroSpaceMicros);
      }
  }
  if (aSendStopBit == StopBit::True) {
    irTx.write(aZeroMarkMicros, 100);  // seems like this is used for stop bits
  }
}
}  // namespace ir