#ifndef _PERIPHERAL_IR_TX_HPP_
#define _PERIPHERAL_IR_TX_HPP_

#include "USART.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "utils/Queue.hpp"
#include "test/test.hpp"

namespace peripheral {
class IrTx {
private:
  const uint32_t carrierPrescaler = 0;
  const uint32_t signalPrescaler = 10;

  hal::Clocks* clocks;
  hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin;
  hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer;
  hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer;
  Queue<uint32_t, IR_TX_BUFFER_SAMPLES> txBuffer;
  bool sending;

public:
  IrTx(
    hal::Clocks* clocks,
    hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin,
    hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer,
    hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer)
    : clocks(clocks), irOutPin(irOutPin), irTxCarrierTimer(irTxCarrierTimer), irTxSignalTimer(irTxSignalTimer) {}

  void initialize(hal::System& system, hal::NVICHal& nvic);

  void reset(uint32_t carrierFrequency);

  void delayMicros(uint32_t microseconds);

  void write(uint32_t t_on, uint32_t t_off);

  void send();

  void handleInterrupt();

  const uint32_t getNumberOfSamplesInBuffer() const;

private:
  void enableGpio(bool enable) const;

  void stop();

  void nextSignal();
};
}  // namespace peripheral

#endif
