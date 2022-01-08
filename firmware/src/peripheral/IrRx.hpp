#ifndef _PERIPHERAL_IR_RX_HPP_
#define _PERIPHERAL_IR_RX_HPP_

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
      hal::Timer<hal::timer::TimerAddress::TIM3Address>& irRxTimer) {
    clocks.enableDMA1Clock();

    irRxPin.enableClock(clocks);
    irRxPin.setSpeed(hal::gpio::Speed::Low);
    irRxPin.setOutputType(hal::gpio::OutputType::PushPull);
    irRxPin.setPull(hal::gpio::Pull::None);
    irRxPin.setAlternate(hal::gpio::Alternate::Alt1);
    irRxPin.setMode(hal::gpio::Mode::Alternate);

    this->irInLedPin->enableClock(clocks);
    this->irInLedPin->resetOutputPin();
    this->irInLedPin->setSpeed(hal::gpio::Speed::Low);
    this->irInLedPin->setOutputType(hal::gpio::OutputType::PushPull);
    this->irInLedPin->setPull(hal::gpio::Pull::None);
    this->irInLedPin->setMode(hal::gpio::Mode::Output);

    irRxTimer.enableClock(clocks);
    irRxTimer.setCounterMode(hal::timer::CounterMode::Up);
    irRxTimer.setClockDivision(hal::timer::ClockDivision::DIV_1);
    irRxTimer.setAutoReload(65535);
    irRxTimer.setRepetitionCounter(0);
    irRxTimer.disableAutoReloadPreload();
    irRxTimer.setTriggerOutput(hal::timer::TriggerOutput::Reset);
    irRxTimer.disableMasterSlaveMode();
    irRxTimer.setInputCaptureActiveInput(hal::timer::Channel::Channel1, hal::timer::InputCaptureActiveInput::DirectTI);
    irRxTimer.setInputCapturePrescaler(hal::timer::Channel::Channel1, hal::timer::InputCapturePrescaler::DIV_1);
    irRxTimer.setInputCaptureFilter(hal::timer::Channel::Channel1, hal::timer::InputCaptureFilter::FDIV1);
    irRxTimer.setInputCapturePolarity(hal::timer::Channel::Channel1, hal::timer::InputCapturePolarity::BothEdges);
    irRxTimer.setPrescaler(SystemCoreClock, 1000000);

    this->irRxDmaChannel->disable();
    this->irRxDmaChannel->setPeripheralRequest(hal::dma::PeripheralRequest::TIM3_CH1);
    this->irRxDmaChannel->setDataTransferDirection(hal::dma::TransferDirection::PeripheralToMemory);
    this->irRxDmaChannel->setChannelPriorityLevel(hal::dma::Priority::Low);
    this->irRxDmaChannel->setMode(hal::dma::Mode::Circular);
    this->irRxDmaChannel->setPeripheralIncrementMode(hal::dma::PeripheralIncrementMode::NoIncrement);
    this->irRxDmaChannel->setMemoryIncrementMode(hal::dma::MemoryIncrementMode::Increment);
    this->irRxDmaChannel->setPeripheralSize(hal::dma::PeripheralSize::HalfWord);
    this->irRxDmaChannel->setMemorySize(hal::dma::MemorySize::HalfWord);
    this->irRxDmaChannel->enableTransferCompleteInterrupt();
    this->irRxDmaChannel->enableTransferErrorInterrupt();
    this->irRxDmaChannel->clearGlobalInterruptFlag();
    this->irRxDmaChannel->setDataLength(IR_RX_BUFFER_SAMPLES);
    this->irRxDmaChannel->setPeripheralAddress((const char*)irRxTimer.getCaptureCompare1RegisterAddress());
    this->irRxDmaChannel->setMemoryAddress(this->buffer);
    this->irRxDmaChannel->setMemorySize(hal::dma::MemorySize::HalfWord);
    memset(this->buffer, 0, sizeof(this->buffer));
    this->irRxDmaChannel->enable();

    irRxTimer.enableCaptureCompareInterrupt(hal::timer::Channel::Channel1);
    irRxTimer.enableCaptureCompareDMARequest(hal::timer::Channel::Channel1);
    irRxTimer.enableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
    irRxTimer.enableCounter();

    this->lastValue = 0;
    this->readIndex = this->getDmaPosition();
    this->lastTick = 0;
  }

  bool read(const hal::Clocks& clocks, uint16_t* result) {
    if (this->getDmaPosition() == this->readIndex) {
      return false;
    }

    // TODO toggle this->irInLedPin

    uint16_t value = this->buffer[this->readIndex++];
    if (this->readIndex >= IR_RX_BUFFER_SAMPLES) {
      this->readIndex = 0;
    }

    *result = value - this->lastValue;
    uint32_t currentTick = clocks.getTickCount();
    bool timeout = (currentTick - this->lastTick) > IR_RX_TIMEOUT_MS;
    this->lastValue = value;
    this->lastTick = currentTick;
    return !timeout;
  }

 private:
  size_t getDmaPosition() const {
    return IR_RX_BUFFER_SAMPLES - this->irRxDmaChannel->getDataLength();
  }
};
}  // namespace peripheral

#endif
