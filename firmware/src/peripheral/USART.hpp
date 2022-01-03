#ifndef _PERIPHERAL_USART_HPP_
#define _PERIPHERAL_USART_HPP_

#include <etl/circular_buffer.h>

#include "hal/USART.hpp"

namespace peripheral {
template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
class USART {
 private:
  hal::USART<TAddress>* usart;
  etl::circular_buffer<uint8_t, TX_BUFFER_SIZE> txBuffer;
  etl::circular_buffer<uint8_t, RX_BUFFER_SIZE> rxBuffer;

 public:
  USART(hal::USART<TAddress>* usart) : usart(usart) {
  }

  const void initialize() const {
    this->usart->enableRxNotEmptyInterrupt();
    this->usart->enableErrorInterrupt();
  }

  const size_t readLine(char* buffer, size_t bufferLen) {
    size_t bufferOffset = 0;
    for (auto it = this->rxBuffer.begin(); it != this->rxBuffer.end(); it++) {
      buffer[bufferOffset] = *it;
      if (*it == '\r' || *it == '\n') {
        for (size_t i = 0; i < bufferOffset; i++) {
          this->rxBuffer.pop();
        }
        buffer[bufferOffset] = '\0';
        return bufferOffset;
      }
      bufferOffset++;
    }
    return 0;
  }

  const void write(const char* buffer) {
    write((const uint8_t*)buffer, strlen(buffer));
  }

  const void write(const uint8_t* buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
      if (this->txBuffer.full()) {
        this->usart->enableTxEmptyInterrupt();
        continue;
      }
      this->txBuffer.push(buffer[i]);
    }
    this->usart->enableTxEmptyInterrupt();
  }

  const void handleInterrupt() {
    if (this->usart->isTxDataRegisterEmptyFlagSet()) {
      if (this->txBuffer.empty()) {
        this->usart->disableTxEmptyInterrupt();
      } else {
        uint8_t b = this->txBuffer[0];
        this->txBuffer.pop();
        // TXE flag will be automatically cleared when writing new data in TDR register
        this->usart->transmitData8(b);
      }
    }

    if (this->usart->isTransmissionCompleteFlagSet()) {
      this->usart->clearTransmissionCompleteFlag();
    }

    if (this->usart->isRxNotEmptyFlagSet()) {
      // RXNE flag is cleared by reading of RDR register
      uint8_t b = this->usart->receiveData8();
      if (this->rxBuffer.full()) {
        Error_Handler();
      }
      this->rxBuffer.push(b);
    }

    if (this->usart->isErrorFlagSet()) {
      while (1)
        ;
    }
  }
};
}  // namespace peripheral

#endif
