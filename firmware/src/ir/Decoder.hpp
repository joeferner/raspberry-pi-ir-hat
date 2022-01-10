#ifndef _IR_DECODER_HPP_
#define _IR_DECODER_HPP_

#include <stddef.h>
#include <stdint.h>

#ifdef TEST
#include "test.hpp"
#endif
#include "Denon.hpp"
#include "NEC.hpp"
#include "ir.hpp"
#include "utils/Queue.hpp"

namespace ir {

class Decoder {
 private:
  DecoderState state;
  DenonDecoder denonDecoder;
  NECDecoder necDecoder;

 public:
  static const uint32_t MAX_QUITE_TIME_MS = 10;

  bool push(uint16_t t, DecoderResults* results) {
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

  void clear() {
    state.clear();
  }
};

}  // namespace ir

#endif
