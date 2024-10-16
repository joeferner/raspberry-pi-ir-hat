#ifndef _PERIPHERAL_IR_TX_HPP_
#define _PERIPHERAL_IR_TX_HPP_

#ifndef TEST
#include "USART.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/IWDG.hpp"
#include "hal/NVIC.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "test/test.hpp"
#include "utils/Queue.hpp"
#endif

namespace peripheral {
#ifndef TEST
class IrTx {
 private:
  const uint32_t carrierPrescaler = 0;
  const uint32_t signalPrescaler = 10;

  hal::Clocks* clocks;
  hal::IWDGHal* iwdg;
  hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin;
  hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer;
  hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer;
  Queue<uint32_t, IR_TX_BUFFER_SAMPLES> txBuffer;

 public:
  IrTx(
      hal::Clocks* clocks,
      hal::IWDGHal* iwdg,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin,
      hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer,
      hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer)
      : clocks(clocks),
        iwdg(iwdg),
        irOutPin(irOutPin),
        irTxCarrierTimer(irTxCarrierTimer),
        irTxSignalTimer(irTxSignalTimer) {
  }

  void initialize(hal::System& system, hal::NVICHal& nvic);

  void reset(uint32_t carrierFrequency);

  void delayMicros(uint32_t microseconds);

  void write(uint32_t t_on, uint32_t t_off);

  void waitForSendToComplete();

  void handleInterrupt();

  uint32_t getNumberOfSamplesInBuffer() const;

  void reloadWatchdogCounter() const;

 private:
  void stop();

  void nextSignal();
};
#else
class IrTx {
 public:
  void reset(uint32_t carrierFrequency) {
  }

  void delayMicros(uint32_t microseconds) {
  }

  void write(uint32_t t_on, uint32_t t_off) {
  }

  void send() {
  }
};
#endif
}  // namespace peripheral

#endif
