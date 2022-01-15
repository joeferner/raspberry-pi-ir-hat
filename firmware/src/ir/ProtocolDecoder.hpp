#ifndef _IR_PROTOCOL_DECODER_HPP_
#define _IR_PROTOCOL_DECODER_HPP_

#include <stddef.h>
#include <stdint.h>

#include "ir.hpp"

namespace ir {

class ProtocolDecoder {
 private:
  /*
   * You can change this value accordingly to the receiver module you use.
   * The required value can be derived from the timings printed here.
   * Keep in mind that the timings may change with the distance
   * between sender and receiver as well as with the ambient light intensity.
   */
  static const uint32_t MARK_EXCESS_MICROS = 20;  // recommended for the cheap VS1838 modules

 protected:
  static const uint32_t MICROS_PER_TICK = 1;

 public:
  virtual bool decode(DecoderState& state, DecoderResults* results) = 0;

 protected:
  /**
   * Adapted from: https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/IRReceive.hpp
   *
   * Decode pulse distance protocols.
   * The mark (pulse) has constant length, the length of the space determines the bit value.
   * Each bit looks like: MARK + SPACE_1 -> 1
   *                 or : MARK + SPACE_0 -> 0
   *
   * Input is     IrReceiver.decodedIRData.rawDataPtr->rawbuf[]
   * Output is    IrReceiver.decodedIRData.decodedRawData
   *
   * @param aStartOffset must point to a mark
   * @return true if decoding was successful
   */
  bool decodePulseDistanceData(
      DecoderState& state,
      uint8_t aNumberOfBits,
      uint8_t aStartOffset,
      uint16_t aBitMarkMicros,
      uint16_t aOneSpaceMicros,
      uint16_t aZeroSpaceMicros,
      Endian endian,
      uint32_t* result_tDecodedData);

  /**
   * Compensate for marks exceeded by demodulator hardware
   */
  bool matchMark(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);

  /**
   * Compensate for spaces shortened by demodulator hardware
   */
  bool matchSpace(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
};

}  // namespace ir

#endif
