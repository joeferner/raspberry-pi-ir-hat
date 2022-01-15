#ifndef _DENON_HPP_
#define _DENON_HPP_

#include "ProtocolDecoder.hpp"
#include "ProtocolEncoder.hpp"

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
class Denon : public ProtocolDecoder, public ProtocolEncoder {
 private:
  static const uint32_t DENON_KHZ = 38;

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
  virtual bool decode(DecoderState& state, DecoderResults* results);

  virtual bool encode(
      peripheral::IrTx& irTx, Protocol protocol, uint32_t aCommand, uint32_t aAddress, uint32_t aNumberOfRepeats) const;
};

}  // namespace ir

#endif
