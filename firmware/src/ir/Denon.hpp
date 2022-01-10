#ifndef _DENON_HPP_
#define _DENON_HPP_

#include "ProtocolDecoder.hpp"

namespace ir {

/**
 * Adapted from: https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/ir_Denon.hpp
 *
 * Denon publish all their IR codes:
 * https://www.google.co.uk/search?q=DENON+MASTER+IR+Hex+Command+Sheet
 * -> http://assets.denon.com/documentmaster/us/denon%20master%20ir%20hex.xls
 *
 * Having looked at the official Denon Pronto sheet and reverse engineered
 * the timing values from it, it is obvious that Denon have a range of
 * different timings and protocols ...the values here work for my AVR-3801 Amp!
 */
class DenonDecoder : public ProtocolDecoder {
 private:
  // MSB first, no start bit, 5 address + 8 command + 2 frame + 1 stop bit - each frame 2 times
  static const uint32_t DENON_ADDRESS_BITS = 5;
  static const uint32_t DENON_COMMAND_BITS = 8;
  static const uint32_t DENON_FRAME_BITS = 2;  // 00/10 for 1. frame Denon/Sharp, inverted for autorepeat frame

  static const uint32_t DENON_BITS =
      (DENON_ADDRESS_BITS + DENON_COMMAND_BITS + DENON_FRAME_BITS);  // 15 - The number of bits in the command
  static const uint32_t DENON_UNIT = 260;

  static const uint32_t DENON_BIT_MARK = DENON_UNIT;          // The length of a Bit:Mark
  static const uint32_t DENON_ONE_SPACE = (7 * DENON_UNIT);   // 1820 // The length of a Bit:Space for 1's
  static const uint32_t DENON_ZERO_SPACE = (3 * DENON_UNIT);  // 780 // The length of a Bit:Space for 0's

  static const uint32_t DENON_AUTO_REPEAT_SPACE =
      45000;  // Every frame is auto repeated with a space period of 45 ms and the command inverted.
  static const uint32_t DENON_REPEAT_PERIOD =
      110000;  // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote
               // control is held down.

  // for old decoder
  static const uint32_t DENON_HEADER_MARK = DENON_UNIT;         // The length of the Header:Mark
  static const uint32_t DENON_HEADER_SPACE = (3 * DENON_UNIT);  // 780 // The length of the Header:Space

 public:
  virtual bool decode(DecoderState& state, DecoderResults* results) {
    // we have no start bit, so check for the exact amount of data bits
    // Check we have the right amount of data (32). The + 2 is for initial gap + stop bit mark
    if (state.buffer.getAvailable() != (2 * DENON_BITS) + 2) {
      return false;
    }

    // Read the bits in
    uint32_t tDecodedData;
    bool decoded = decodePulseDistanceData(
        state, DENON_BITS, 1, DENON_BIT_MARK, DENON_ONE_SPACE, DENON_ZERO_SPACE, Endian::MSB, &tDecodedData);
    if (!decoded) {
      return false;
    }

    // Check for stop mark
    if (!matchMark(state.buffer[(2 * DENON_BITS) + 1], DENON_HEADER_MARK)) {
      return false;
    }

    // Success
    results->endian = Endian::MSB;
    uint8_t tFrameBits = tDecodedData & 0x03;
    results->command = tDecodedData >> DENON_FRAME_BITS;
    results->address = results->command >> DENON_COMMAND_BITS;
    uint8_t tCommand = results->command & 0xFF;
    results->command = tCommand;

    // check for autorepeated inverted command
    if (state.buffer[0] < ((DENON_AUTO_REPEAT_SPACE + (DENON_AUTO_REPEAT_SPACE / 4)) / MICROS_PER_TICK)) {
      state.repeatCount++;
      if (tFrameBits == 0x3 || tFrameBits == 0x1) {
        // We are in the auto repeated frame with the inverted command
        results->autoRepeat = true;
        ;
        // Check parity of consecutive received commands. There is no parity in one data set.
        uint8_t tLastCommand = state.lastDecodedCommand;
        if (tLastCommand != (uint8_t)(~tCommand)) {
          results->parityFailed = true;
        }
        // always take non inverted command
        results->command = tLastCommand;
      }
      if (state.repeatCount > 1) {
        results->repeat = true;
      }
    } else {
      state.repeatCount = 0;
    }

    results->numberOfBits = DENON_BITS;
    if (tFrameBits == 1 || tFrameBits == 2) {
      results->protocol = Protocol::Sharp;
    } else {
      results->protocol = Protocol::Denon;
    }
    return true;
  }
};

}  // namespace ir

#endif
