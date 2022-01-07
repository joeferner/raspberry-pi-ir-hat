#ifndef _PERIPHERAL_USART_HPP_
#define _PERIPHERAL_USART_HPP_

#include <etl/circular_buffer.h>

#include "hal/Clocks.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "hal/USART.hpp"

namespace peripheral {
class USARTWriter {
 public:
  virtual void write(const char* buffer) = 0;
  virtual void write(const uint8_t* buffer, size_t length) = 0;
};

template <hal::usart::USARTAddress TAddress, size_t TX_BUFFER_SIZE, size_t RX_BUFFER_SIZE>
class USART : public USARTWriter {
 private:
  hal::USART<TAddress>* usart;
  etl::circular_buffer<uint8_t, TX_BUFFER_SIZE> txBuffer;
  etl::circular_buffer<uint8_t, RX_BUFFER_SIZE> rxBuffer;

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
      hal::usart::StopBits stopBits) const {
    clocks.enableUSART1Clock();
    clocks.enableGPIOBClock();

    usart1RxPin.setSpeed(hal::gpio::Speed::Low);
    usart1RxPin.setOutputType(hal::gpio::OutputType::PushPull);
    usart1RxPin.setPull(hal::gpio::Pull::None);
    usart1RxPin.setAlternate(hal::gpio::Alternate::Alt0);
    usart1RxPin.setMode(hal::gpio::Mode::Alternate);

    usart1TxPin.setSpeed(hal::gpio::Speed::Low);
    usart1TxPin.setOutputType(hal::gpio::OutputType::PushPull);
    usart1TxPin.setPull(hal::gpio::Pull::None);
    usart1TxPin.setAlternate(hal::gpio::Alternate::Alt0);
    usart1TxPin.setMode(hal::gpio::Mode::Alternate);

    nvic.setPriority(hal::nvic::IRQnType::USART1_Irq, 0);
    nvic.enableInterrupt(hal::nvic::IRQnType::USART1_Irq);

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

  void write(const char* buffer) {
    write((const uint8_t*)buffer, strlen(buffer));
  }

  void write(const uint8_t* buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
      if (this->txBuffer.full()) {
        this->usart->enableTxEmptyInterrupt();
        continue;
      }
      this->txBuffer.push(buffer[i]);
    }
    this->usart->enableTxEmptyInterrupt();
  }

  void handleInterrupt() {
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
