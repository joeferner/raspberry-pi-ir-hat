
#ifndef _USART_OUTPUT_HPP_
#define _USART_OUTPUT_HPP_

#include "peripheral/USART.hpp"

class BothUSARTWriter : public peripheral::USARTWriter {
 public:
  virtual void write(const char* buffer);
  virtual void write(const uint8_t* buffer, size_t length);
  virtual void writef(const char* format, ...);
  virtual void vwritef(const char* format, va_list args);
};

extern BothUSARTWriter usartOutput;

#endif
