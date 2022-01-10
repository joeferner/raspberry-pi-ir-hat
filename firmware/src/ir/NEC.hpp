#ifndef _NEC_HPP_
#define _NEC_HPP_

#include "LongUnion.h"
#include "ProtocolDecoder.hpp"

namespace ir {

/**
 * Adapted from: https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/ir_NEC.hpp
 *
 * see: https://www.sbprojects.net/knowledge/ir/nec.php
 * for Apple see https://en.wikipedia.org/wiki/Apple_Remote
 * ONKYO like NEC but 16 independent command bits
 */
class NECDecoder : public ProtocolDecoder {
 private:
  // LSB first, 1 start bit + 16 bit address + 8 bit command + 8 bit inverted command + 1 stop bit.
  static const uint32_t NEC_ADDRESS_BITS = 16;  // 16 bit address or 8 bit address and 8 bit inverted address
  static const uint32_t NEC_COMMAND_BITS = 16;  // Command and inverted command

  static const uint32_t NEC_BITS = (NEC_ADDRESS_BITS + NEC_COMMAND_BITS);
  static const uint32_t NEC_UNIT = 560;  // 21.28 periods of 38 kHz

  static const uint32_t NEC_HEADER_MARK = (16 * NEC_UNIT);  // 9000
  static const uint32_t NEC_HEADER_SPACE = (8 * NEC_UNIT);  // 4500

  static const uint32_t NEC_BIT_MARK = NEC_UNIT;
  static const uint32_t NEC_ONE_SPACE = (3 * NEC_UNIT);  // 1690
  static const uint32_t NEC_ZERO_SPACE = NEC_UNIT;

  static const uint32_t NEC_REPEAT_HEADER_SPACE = (4 * NEC_UNIT);  // 2250

  static const uint32_t NEC_AVERAGE_DURATION = 62000;  // NEC_HEADER_MARK + NEC_HEADER_SPACE + 32 * 2,5 * NEC_UNIT +
                                                       // NEC_UNIT // 2.5 because we assume more zeros than ones
  static const uint32_t NEC_REPEAT_DURATION = (NEC_HEADER_MARK + NEC_REPEAT_HEADER_SPACE + NEC_BIT_MARK);  // 12 ms
  static const uint32_t NEC_REPEAT_PERIOD =
      110000;  // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote
               // control is held down.
  static const uint32_t NEC_REPEAT_SPACE = (NEC_REPEAT_PERIOD - NEC_AVERAGE_DURATION);  // 48 ms

  static const uint32_t APPLE_ADDRESS = 0x87EE;

 public:
  virtual bool decode(DecoderState& state, DecoderResults* results) {
    // Check we have the right amount of data (68). The +4 is for initial gap, start bit mark and space + stop bit mark.
    if (state.buffer.getAvailable() != ((2 * NEC_BITS) + 4) && (state.buffer.getAvailable() != 4)) {
      return false;
    }

    // Check header "mark" this must be done for repeat and data
    if (!matchMark(state.buffer[1], NEC_HEADER_MARK)) {
      return false;
    }

    // Check for repeat - here we have another header space length
    if (state.buffer.getAvailable() == 4) {
      if (matchSpace(state.buffer[2], NEC_REPEAT_HEADER_SPACE) && matchMark(state.buffer[3], NEC_BIT_MARK)) {
        results->repeat = true;
        results->endian = Endian::LSB;
        results->address = state.lastDecodedAddress;
        results->command = state.lastDecodedCommand;
        results->protocol = state.lastDecodedProtocol;
        return true;
      }
      return false;
    }

    // Check command header space
    if (!matchSpace(state.buffer[2], NEC_HEADER_SPACE)) {
      return false;
    }

    uint32_t tDecodedData;
    if (!decodePulseDistanceData(
            state, NEC_BITS, 3, NEC_BIT_MARK, NEC_ONE_SPACE, NEC_ZERO_SPACE, Endian::LSB, &tDecodedData)) {
      return false;
    }

    // Stop bit
    if (!matchMark(state.buffer[3 + (2 * NEC_BITS)], NEC_BIT_MARK)) {
      return false;
    }

    // Success
    LongUnion tValue;
    tValue.ULong = tDecodedData;
    results->command = tValue.UByte.MidHighByte;  // 8 bit
    // Address
    if (tValue.UWord.LowWord == APPLE_ADDRESS) {
      /*
       * Apple
       */
      results->protocol = Protocol::Apple;
      results->address = tValue.UByte.HighByte;
    } else {
      /*
       * NEC
       */
      if (tValue.UByte.LowByte == (uint8_t)(~tValue.UByte.MidLowByte)) {
        // standard 8 bit address NEC protocol
        results->address = tValue.UByte.LowByte;  // first 8 bit
      } else {
        // extended NEC protocol
        results->address = tValue.UWord.LowWord;  // first 16 bit
      }
      // Check for command if it is 8 bit NEC or 16 bit ONKYO
      if (tValue.UByte.MidHighByte == (uint8_t)(~tValue.UByte.HighByte)) {
        results->protocol = Protocol::NEC;
      } else {
        results->protocol = Protocol::Onkyo;
        results->command = tValue.UWord.HighWord;  // 16 bit command
      }
    }
    results->numberOfBits = NEC_BITS;

    return true;
  }
};

}  // namespace ir

#endif
