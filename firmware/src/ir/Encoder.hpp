#ifndef _IR_ENCODER_HPP_
#define _IR_ENCODER_HPP_

#include <stddef.h>
#include <stdint.h>

#include "test/test.hpp"
#include "Denon.hpp"
#include "NEC.hpp"
#include "ir.hpp"
#include "peripheral/IrTx.hpp"
#include "utils/Queue.hpp"

namespace ir {

class Encoder {
 private:
  Denon denonEncoder;
  NEC necEncoder;

 public:
  bool send(peripheral::IrTx& irTx, Protocol protocol, uint32_t address, uint32_t command, uint32_t numberOfRepeats) {
    switch (protocol) {
      case Protocol::Denon:
      case Protocol::Sharp:
        return denonEncoder.encode(irTx, protocol, address, command, numberOfRepeats);

      case Protocol::Apple:
      case Protocol::NEC:
      case Protocol::Onkyo:
        return necEncoder.encode(irTx, protocol, address, command, numberOfRepeats);

      default:
        return false;
    }
  }
};

}  // namespace ir

#endif
