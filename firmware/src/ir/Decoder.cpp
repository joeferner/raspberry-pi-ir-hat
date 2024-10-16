#include "Decoder.hpp"

namespace ir {

bool Decoder::push(uint16_t t, DecoderResults* results) {
  state.buffer.push(t);

  if (denonDecoder.decode(state, results) || necDecoder.decode(state, results)) {
    state.buffer.clear();
    state.lastDecodedAddress = results->address;
    state.lastDecodedCommand = results->command;
    state.lastDecodedProtocol = results->protocol;
    return true;
  }
  return false;
}

void Decoder::clear() {
  state.clear();
}

}  // namespace ir
