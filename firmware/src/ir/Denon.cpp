#include "Denon.hpp"

namespace ir {

bool Denon::decode(DecoderState& state, DecoderResults* results) {
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

bool Denon::encode(
    peripheral::IrTx& irTx, Protocol protocol, uint32_t aAddress, uint32_t aCommand, uint32_t aNumberOfRepeats) const {
  irTx.reset(DENON_KHZ * 1000);

  // Shift command and add frame marker
  uint16_t tCommand = aCommand << DENON_FRAME_BITS;  // the lowest bits are 00 for Denon and 10 for Sharp
  if (protocol == Protocol::Sharp) {
    tCommand |= 0x02;
  }
  uint16_t tData = tCommand | ((uint16_t)aAddress << (DENON_COMMAND_BITS + DENON_FRAME_BITS));
  uint16_t tInvertedData = ((~tCommand) & 0x3FF) | (uint16_t)aAddress << (DENON_COMMAND_BITS + DENON_FRAME_BITS);

  uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
  while (tNumberOfCommands > 0) {
    irTx.reloadWatchdogCounter();

    // Data
    sendPulseDistanceWidthData(
        irTx,
        DENON_BIT_MARK,
        DENON_ONE_SPACE,
        DENON_BIT_MARK,
        DENON_ZERO_SPACE,
        tData,
        DENON_BITS,
        Endian::MSB,
        StopBit::True);
    irTx.waitForSendToComplete();

    // Inverted autorepeat frame
    irTx.delayMicros(DENON_AUTO_REPEAT_SPACE);
    sendPulseDistanceWidthData(
        irTx,
        DENON_BIT_MARK,
        DENON_ONE_SPACE,
        DENON_BIT_MARK,
        DENON_ZERO_SPACE,
        tInvertedData,
        DENON_BITS,
        Endian::MSB,
        StopBit::True);
    irTx.waitForSendToComplete();

    tNumberOfCommands--;
    // skip last delay!
    if (tNumberOfCommands > 0) {
      // send repeated command with a fixed space gap
      irTx.delayMicros(DENON_AUTO_REPEAT_SPACE);
    }
  }

  return true;
}

}  // namespace ir
