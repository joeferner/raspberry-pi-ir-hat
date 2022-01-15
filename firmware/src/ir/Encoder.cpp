#include "Encoder.hpp"

namespace ir {

bool Encoder::send(
    peripheral::IrTx& irTx, Protocol protocol, uint32_t address, uint32_t command, uint32_t numberOfRepeats) const {
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

}  // namespace ir
