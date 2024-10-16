#ifndef _NEC_HPP_
#define _NEC_HPP_

#include "LongUnion.h"
#include "ProtocolDecoder.hpp"
#include "ProtocolEncoder.hpp"
#include "peripheral/IrTx.hpp"

namespace ir {

/**
 * Adapted from: https://github.com/Arduino-IRremote/Arduino-IRremote/blob/master/src/ir_NEC.hpp
 *
 * see: https://www.sbprojects.net/knowledge/ir/nec.php
 * for Apple see https://en.wikipedia.org/wiki/Apple_Remote
 * ONKYO like NEC but 16 independent command bits
 */
class NEC : public ProtocolDecoder, public ProtocolEncoder {
 private:
  static const uint32_t NEC_KHZ = 38;

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
  virtual bool decode(DecoderState& state, DecoderResults* results);

  virtual bool encode(
      peripheral::IrTx& irTx, Protocol protocol, uint32_t aAddress, uint32_t aCommand, uint32_t aNumberOfRepeats) const;

 private:
  /*
   * Repeat commands should be sent in a 110 ms raster.
   * There is NO delay after the last sent repeat!
   * https://www.sbprojects.net/knowledge/ir/nec.php
   * @param aIsRepeat if true, send only one repeat frame without leading and trailing space
   */
  bool sendNEC(
      peripheral::IrTx& irTx, uint32_t aCommand, uint32_t aAddress, uint32_t aNumberOfRepeats, bool aIsRepeat) const;

  bool sendNECRaw(peripheral::IrTx& irTx, uint32_t aRawData, uint_fast8_t aNumberOfRepeats, bool aIsRepeat) const;

  void sendNECRepeat(peripheral::IrTx& irTx) const;
};

}  // namespace ir

#endif
