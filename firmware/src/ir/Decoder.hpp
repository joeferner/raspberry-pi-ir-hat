#ifndef _IR_DECODER_HPP_
#define _IR_DECODER_HPP_

#include <stddef.h>
#include <stdint.h>

#include "Denon.hpp"
#include "NEC.hpp"
#include "ir.hpp"
#include "test/test.hpp"
#include "utils/Queue.hpp"

namespace ir {

class Decoder {
 private:
  DecoderState state;
  Denon denonDecoder;
  NEC necDecoder;

 public:
  static const uint32_t MAX_QUITE_TIME_MS = 10;

  bool push(uint16_t t, DecoderResults* results);

  void clear();
};

}  // namespace ir

#endif
