#ifndef _PERIPHERAL_IR_RX_HPP_
#define _PERIPHERAL_IR_RX_HPP_

#include "hal/Clocks.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "hal/Timer.hpp"

namespace peripheral {
class IrRx {
 private:
  hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5>* irRxDmaChannel;
  hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin7>* irInLedPin;
  uint16_t buffer[IR_RX_BUFFER_SAMPLES];
  uint32_t readIndex;
  uint16_t lastValue;
  uint32_t lastTick;

 public:
  IrRx(
      hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5>* irRxDmaChannel,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin7>* irInLedPin)
      : irRxDmaChannel(irRxDmaChannel), irInLedPin(irInLedPin) {
  }

  void initialize(
      hal::NVICHal& nvic,
      hal::Clocks& clocks,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin6>& irRxPin,
      hal::Timer<hal::timer::TimerAddress::TIM3Address>& irRxTimer);

  bool read(const hal::Clocks& clocks, uint16_t* result);

 private:
  size_t getDmaPosition() const;
};
}  // namespace peripheral

#endif
