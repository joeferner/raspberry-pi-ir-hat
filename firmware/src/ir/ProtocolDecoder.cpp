#include "ProtocolDecoder.hpp"

namespace ir {

/*
 * Pulse parms are ((X*50)-100) for the Mark and ((X*50)+100) for the Space.
 * First MARK is the one after the long gap
 * Pulse parameters in uSec
 */
/** Relative tolerance (in percent) for some comparisons on measured data. */
#define TOLERANCE 45

/** Lower tolerance for comparison of measured data */
#define LTOL (100 - TOLERANCE)
/** Upper tolerance for comparison of measured data */
#define UTOL (100 + TOLERANCE)

#define TICKS_LOW(us) ((uint16_t)((long)(us)*LTOL / (MICROS_PER_TICK * 100)))
#define TICKS_HIGH(us) ((uint16_t)((long)(us)*UTOL / (MICROS_PER_TICK * 100) + 1))

bool ProtocolDecoder::decodePulseDistanceData(
    DecoderState& state,
    uint8_t aNumberOfBits,
    uint8_t aStartOffset,
    uint16_t aBitMarkMicros,
    uint16_t aOneSpaceMicros,
    uint16_t aZeroSpaceMicros,
    Endian endian,
    uint32_t* result_tDecodedData) {
  auto tRawBufPointer = state.buffer.begin(aStartOffset);
  uint32_t tDecodedData = 0;

  if (endian == Endian::MSB) {
    for (uint_fast8_t i = 0; i < aNumberOfBits; i++) {
      // Check for constant length mark
      if (!matchMark(*tRawBufPointer, aBitMarkMicros)) {
        return false;
      }
      tRawBufPointer++;

      // Check for variable length space indicating a 0 or 1
      if (matchSpace(*tRawBufPointer, aOneSpaceMicros)) {
        tDecodedData = (tDecodedData << 1) | 1;
      } else if (matchSpace(*tRawBufPointer, aZeroSpaceMicros)) {
        tDecodedData = (tDecodedData << 1) | 0;
      } else {
        return false;
      }
      tRawBufPointer++;
    }

  } else {
    for (uint32_t tMask = 1UL; aNumberOfBits > 0; tMask <<= 1, aNumberOfBits--) {
      // Check for constant length mark
      if (!matchMark(*tRawBufPointer, aBitMarkMicros)) {
        return false;
      }
      tRawBufPointer++;

      // Check for variable length space indicating a 0 or 1
      if (matchSpace(*tRawBufPointer, aOneSpaceMicros)) {
        tDecodedData |= tMask;  // set the bit
      } else if (matchSpace(*tRawBufPointer, aZeroSpaceMicros)) {
        // do not set the bit
      } else {
        return false;
      }
      tRawBufPointer++;
    }
  }
  *result_tDecodedData = tDecodedData;
  return true;
}

bool ProtocolDecoder::matchMark(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros) {
  // compensate for marks exceeded by demodulator hardware
  uint16_t low = TICKS_LOW(aMatchValueMicros + MARK_EXCESS_MICROS);
  uint16_t high = TICKS_HIGH(aMatchValueMicros + MARK_EXCESS_MICROS);
  return ((aMeasuredTicks >= low) && (aMeasuredTicks <= high));
}

bool ProtocolDecoder::matchSpace(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros) {
  // compensate for spaces shortened by demodulator hardware
  uint16_t low = TICKS_LOW(aMatchValueMicros - MARK_EXCESS_MICROS);
  uint16_t high = TICKS_HIGH(aMatchValueMicros - MARK_EXCESS_MICROS);
  return ((aMeasuredTicks >= low) && (aMeasuredTicks <= high));
}

}  // namespace ir
