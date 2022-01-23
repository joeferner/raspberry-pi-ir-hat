#ifndef _PERIPHERAL_USART_HPP_
#define _PERIPHERAL_USART_HPP_

#include <stdlib.h>

#include <cstdarg>

#include "hal/Clocks.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "hal/USART.hpp"
#include "utils/Queue.hpp"

namespace peripheral {
class USARTWriter {
 public:
  virtual void write(const char* buffer) = 0;
  virtual void write(const uint8_t* buffer, size_t length) = 0;
  virtual void writef(const char* format, ...) = 0;
  virtual void vwritef(const char* format, va_list args) = 0;
};

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
class USART : public USARTWriter {
 private:
  hal::USART<TAddress>* usart;
  Queue<char, TX_BUFFER_SIZE> txBuffer;
  Queue<char, RX_BUFFER_SIZE> rxBuffer;

 public:
  USART(hal::USART<TAddress>* usart) : usart(usart) {
  }

  void initialize(
      hal::Clocks& clocks,
      hal::NVICHal& nvic,
      const hal::RCCHal& rcc,
      hal::USART<hal::usart::USARTAddress::USART1Address>& usart1,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin6>& usart1TxPin,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin7>& usart1RxPin,
      uint32_t baudRate,
      hal::usart::DataWidth dataWidth,
      hal::usart::Parity parity,
      hal::usart::StopBits stopBits) const;

  void initialize(
      hal::Clocks& clocks,
      hal::NVICHal& nvic,
      const hal::RCCHal& rcc,
      hal::USART<hal::usart::USARTAddress::USART2Address>& usart2,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin2> usart2TxPin,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin3> usart2RxPin,
      uint32_t baudRate,
      hal::usart::DataWidth dataWidth,
      hal::usart::Parity parity,
      hal::usart::StopBits stopBits) const;

  const bool readLine(char* buffer, size_t bufferLen);

  virtual void write(const char* buffer);

  virtual void write(const uint8_t* buffer, size_t length);

  virtual void writef(const char* format, ...);

  virtual void vwritef(const char* format, va_list args);

  void handleInterrupt();
};
}  // namespace peripheral

#endif
