#include "USART.hpp"

namespace peripheral
{

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::initialize(
  hal::Clocks& clocks,
  hal::NVICHal& nvic,
  const hal::RCCHal& rcc,
  hal::USART<hal::usart::USARTAddress::USART1Address>& usart1,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin6>& usart1TxPin,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin7>& usart1RxPin,
  uint32_t baudRate,
  hal::usart::DataWidth dataWidth,
  hal::usart::Parity parity,
  hal::usart::StopBits stopBits) const {
  usart1RxPin.enableClock(clocks);
  usart1RxPin.setSpeed(hal::gpio::Speed::Low);
  usart1RxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart1RxPin.setPull(hal::gpio::Pull::None);
  usart1RxPin.setAlternate(hal::gpio::Alternate::Alt0);
  usart1RxPin.setMode(hal::gpio::Mode::Alternate);

  usart1TxPin.enableClock(clocks);
  usart1TxPin.setSpeed(hal::gpio::Speed::Low);
  usart1TxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart1TxPin.setPull(hal::gpio::Pull::None);
  usart1TxPin.setAlternate(hal::gpio::Alternate::Alt0);
  usart1TxPin.setMode(hal::gpio::Mode::Alternate);

  nvic.setPriority(hal::nvic::IRQnType::USART1_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::USART1_Irq);

  usart1.enableClock(clocks);
  usart1.setDataWidth(dataWidth);
  usart1.setParity(parity);
  usart1.setStopBits(stopBits);
  usart1.setOverSampling(hal::usart::OverSampling::OverSampling16);
  usart1.setTransferDirection(hal::usart::TransferDirection::TxRx);
  usart1.setHardwareFlowControl(hal::usart::HardwardFlowControl::None);
  usart1.setPrescaler(hal::usart::Prescaler::DIV_1);
  usart1.setBaudRate(rcc, baudRate);

  usart1.setTXFIFOThreshold(hal::usart::FIFOThreshold::Threshold1_8);
  usart1.setRXFIFOThreshold(hal::usart::FIFOThreshold::Threshold1_8);
  usart1.disableFIFO();
  usart1.configAsyncMode();

  this->usart->enableRxNotEmptyInterrupt();
  this->usart->enableErrorInterrupt();

  usart1.enable();
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::initialize(
  hal::Clocks& clocks,
  hal::NVICHal& nvic,
  const hal::RCCHal& rcc,
  hal::USART<hal::usart::USARTAddress::USART2Address>& usart2,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin2> usart2TxPin,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin3> usart2RxPin,
  uint32_t baudRate,
  hal::usart::DataWidth dataWidth,
  hal::usart::Parity parity,
  hal::usart::StopBits stopBits) const {
  usart2RxPin.enableClock(clocks);
  usart2RxPin.setSpeed(hal::gpio::Speed::Low);
  usart2RxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart2RxPin.setPull(hal::gpio::Pull::None);
  usart2RxPin.setAlternate(hal::gpio::Alternate::Alt1);
  usart2RxPin.setMode(hal::gpio::Mode::Alternate);

  usart2TxPin.enableClock(clocks);
  usart2TxPin.setSpeed(hal::gpio::Speed::Low);
  usart2TxPin.setOutputType(hal::gpio::OutputType::PushPull);
  usart2TxPin.setPull(hal::gpio::Pull::None);
  usart2TxPin.setAlternate(hal::gpio::Alternate::Alt1);
  usart2TxPin.setMode(hal::gpio::Mode::Alternate);

  nvic.setPriority(hal::nvic::IRQnType::USART2_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::USART2_Irq);

  usart2.enableClock(clocks);
  usart2.setDataWidth(dataWidth);
  usart2.setParity(parity);
  usart2.setStopBits(stopBits);
  usart2.setOverSampling(hal::usart::OverSampling::OverSampling16);
  usart2.setTransferDirection(hal::usart::TransferDirection::TxRx);
  usart2.setHardwareFlowControl(hal::usart::HardwardFlowControl::None);
  usart2.setPrescaler(hal::usart::Prescaler::DIV_1);
  usart2.setBaudRate(rcc, baudRate);
  usart2.configAsyncMode();

  this->usart->enableRxNotEmptyInterrupt();
  this->usart->enableErrorInterrupt();

  usart2.enable();
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
const bool USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::readLine(char* buffer, size_t bufferLen) {
  size_t bufferOffset = 0;
  for (auto it = this->rxBuffer.begin(); it != this->rxBuffer.end(); it++) {
    buffer[bufferOffset] = *it;
    if (*it == '\r' || *it == '\n') {
      for (size_t i = 0; i <= bufferOffset; i++) {
        if (this->rxBuffer.pop() != buffer[i]) {
          assert_param(0);
        }
      }
      buffer[bufferOffset] = '\0';
      return true;
    }
    bufferOffset++;
  }
  if (this->rxBuffer.isFull()) {
    this->rxBuffer.pop();
  }
  return false;
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::write(const char* buffer) {
  write((const uint8_t*)buffer, strlen(buffer));
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::write(const uint8_t* buffer, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (this->txBuffer.isFull()) {
      this->usart->enableTxEmptyInterrupt();
      continue;
    }
    this->txBuffer.push(buffer[i]);
  }
  this->usart->enableTxEmptyInterrupt();
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::writef(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vwritef(format, args);
  va_end(args);
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::vwritef(const char* format, va_list args) {
  char temp[20];
  while (*format != '\0') {
    if (*format == '%') {
      format++;
      if (*format == '\0') {
        assert_param(0);
        return;
      }
      if (*format == 'd') {
        int i = va_arg(args, int);
        itoa(i, temp, 10);
        write(temp);
      } else if (*format == 's') {
        const char* p = va_arg(args, const char*);
        write(p);
      }
    } else {
      if (this->txBuffer.isFull()) {
        this->usart->enableTxEmptyInterrupt();
      }
      this->txBuffer.push(*format);
    }
    format++;
  }
  this->usart->enableTxEmptyInterrupt();
}

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
void USART<TAddress, TX_BUFFER_SIZE, RX_BUFFER_SIZE>::handleInterrupt() {
  if (this->usart->isTxDataRegisterEmptyFlagSet()) {
    if (this->txBuffer.isEmpty()) {
      this->usart->disableTxEmptyInterrupt();
    } else {
      uint8_t b = this->txBuffer.pop();
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
    this->rxBuffer.push(b);
  }

  if (this->usart->isOverrunErrorFlagSet()) {
    this->usart->clearOverrunErrorFlag();
    this->write("-ERR USART:OVR\n");
  }

  if (this->usart->isFramingErrorFlagSet()) {
    this->usart->clearFramingErrorFlag();
    this->write("-ERR USART:FE\n");
  }

  if (this->usart->isNoiseErrorFlagSet()) {
    this->usart->clearNoiseErrorFlag();
    this->write("-ERR USART:NE\n");
  }
}

template class USART<hal::usart::USARTAddress::USART1Address, USART_TX_BUFFER_SIZE, USART_RX_BUFFER_SIZE>;
template class USART<hal::usart::USARTAddress::USART2Address, USART_TX_BUFFER_SIZE, USART_RX_BUFFER_SIZE>;

}  // namespace peripheral
