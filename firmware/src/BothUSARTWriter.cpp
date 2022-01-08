#include "BothUSARTWriter.hpp"

#include "globals.hpp"

BothUSARTWriter usartOutput;

void BothUSARTWriter::write(const char* buffer) {
  rpiUsart.write(buffer);
  debugUsart.write(buffer);
}

void BothUSARTWriter::write(const uint8_t* buffer, size_t length) {
  rpiUsart.write(buffer, length);
  debugUsart.write(buffer, length);
}
