#ifndef _IR_ENCODER_HPP_
#define _IR_ENCODER_HPP_

#include <stddef.h>
#include <stdint.h>

#include "Denon.hpp"
#include "NEC.hpp"
#include "ir.hpp"
#include "peripheral/IrTx.hpp"
#include "test/test.hpp"
#include "utils/Queue.hpp"

namespace ir {

class Encoder {
 private:
  Denon denonEncoder;
  NEC necEncoder;

 public:
  bool send(
      peripheral::IrTx& irTx, Protocol protocol, uint32_t address, uint32_t command, uint32_t numberOfRepeats) const;
};

}  // namespace ir

#endif
