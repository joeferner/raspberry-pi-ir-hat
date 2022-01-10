#include "BothUSARTWriter.hpp"

#include <cstdarg>

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

void BothUSARTWriter::writef(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vwritef(format, args);
  va_end(args);
}

void BothUSARTWriter::vwritef(const char* format, va_list args) {
  rpiUsart.vwritef(format, args);
  debugUsart.vwritef(format, args);
}