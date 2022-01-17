#include "NEC.hpp"

namespace ir {

bool NEC::decode(DecoderState& state, DecoderResults* results) {
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

bool NEC::encode(
    peripheral::IrTx& irTx, Protocol protocol, uint32_t aAddress, uint32_t aCommand, uint32_t aNumberOfRepeats) const {
  switch (protocol) {
    case Protocol::NEC:
      return sendNEC(irTx, aCommand, aAddress, aNumberOfRepeats, false);
    case Protocol::Apple:
      // TODO apple
      return false;
    case Protocol::Onkyo:
      // TODO apple
      return false;

    default:
      return false;
  }
}

bool NEC::sendNEC(
    peripheral::IrTx& irTx, uint32_t aCommand, uint32_t aAddress, uint32_t aNumberOfRepeats, bool aIsRepeat) const {
  LongUnion tRawData;

  // Address 16 bit LSB first
  if ((aAddress & 0xFF00) == 0) {
    // assume 8 bit address -> send 8 address bits and then 8 inverted address bits LSB first
    tRawData.UByte.LowByte = aAddress;
    tRawData.UByte.MidLowByte = ~tRawData.UByte.LowByte;
  } else {
    tRawData.UWord.LowWord = aAddress;
  }

  // send 8 command bits and then 8 inverted command bits LSB first
  tRawData.UByte.MidHighByte = aCommand;
  tRawData.UByte.HighByte = ~aCommand;

  return sendNECRaw(irTx, tRawData.ULong, aNumberOfRepeats, aIsRepeat);
}

bool NEC::sendNECRaw(peripheral::IrTx& irTx, uint32_t aRawData, uint_fast8_t aNumberOfRepeats, bool aIsRepeat) const {
  if (aIsRepeat) {
    sendNECRepeat(irTx);
    return true;
  }
  // Set IR carrier frequency
  irTx.reset(NEC_KHZ * 1000);

  // Header
  irTx.write(NEC_HEADER_MARK, NEC_HEADER_SPACE);

  // LSB first + stop bit
  sendPulseDistanceWidthData(
      irTx, NEC_BIT_MARK, NEC_ONE_SPACE, NEC_BIT_MARK, NEC_ZERO_SPACE, aRawData, NEC_BITS, Endian::LSB, StopBit::True);
  irTx.sendAndWait();

  for (uint32_t i = 0; i < aNumberOfRepeats; ++i) {
    // send repeat in a 110 ms raster
    if (i == 0) {
      irTx.delayMicros(NEC_REPEAT_SPACE);
    } else {
      irTx.delayMicros(NEC_REPEAT_PERIOD - NEC_REPEAT_DURATION);
    }
    // send repeat
    sendNECRepeat(irTx);
  }

  return true;
}

void NEC::sendNECRepeat(peripheral::IrTx& irTx) const {
  irTx.reset(NEC_KHZ);
  irTx.write(NEC_HEADER_MARK, NEC_REPEAT_HEADER_SPACE);
  irTx.write(NEC_BIT_MARK, 100);
  irTx.send();
}

}  // namespace ir
