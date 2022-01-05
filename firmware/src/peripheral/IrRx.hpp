#ifndef _PERIPHERAL_HPP_
#define _PERIPHERAL_HPP_

#include "hal/DMA.hpp"
#include "hal/Timer.hpp"

namespace peripheral {
class IrRx {
private:
  hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5>* irRxDmaChannel;
  uint16_t buffer[IR_RX_BUFFER_SAMPLES];
  uint32_t readIndex;
  uint16_t lastValue;
  uint32_t lastTick;

public:
  IrRx(hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5>* irRxDmaChannel)
    : irRxDmaChannel(irRxDmaChannel) {
  }

  const void initialize(hal::Timer<hal::timer::TimerAddress::TIM3Address>& irRxTimer) {
    irRxTimer.setPrescaler(SystemCoreClock, 1000000);

    this->irRxDmaChannel->disable();
    this->irRxDmaChannel->enableTransferCompleteInterrupt();
    this->irRxDmaChannel->enableTransferErrorInterrupt();
    this->irRxDmaChannel->clearGlobalInterruptFlag();
    this->irRxDmaChannel->setDataLength(IR_RX_BUFFER_SAMPLES);
    this->irRxDmaChannel->setPeripheralAddress((const char*)irRxTimer.getCaptureCompare1RegisterAddress());
    this->irRxDmaChannel->setMemoryAddress(this->buffer);
    this->irRxDmaChannel->setMemorySize(hal::dma::MemorySize::HalfWord);
    memset(this->buffer, 0, sizeof(this->buffer));
    this->irRxDmaChannel->enable();

    this->lastValue = 0;
    this->readIndex = this->getDmaPosition();
    this->lastTick = 0;

    irRxTimer.enableCaptureCompareInterrupt(hal::timer::Channel::Channel1);
    irRxTimer.enableCaptureCompareDMARequest(hal::timer::Channel::Channel1);
    irRxTimer.enableChannel(hal::timer::ChannelN::Channel1);
    irRxTimer.enableCounter();
  }

  const bool read(const hal::Clocks& clocks, uint16_t* result) {
    if (this->getDmaPosition() == this->readIndex) {
      return false;
    }

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

  const void handleInterrupt() const {
    // TODO do we need any of this?
    if (this->irRxDmaChannel->isTransferCompleteFlagSet()) {
      this->irRxDmaChannel->clearGlobalInterruptFlag();
    }

    if (this->irRxDmaChannel->isGlobalInterruptFlagSet()) {
      this->irRxDmaChannel->clearGlobalInterruptFlag();
    }
  }

private:
  const size_t getDmaPosition() const {
    return IR_RX_BUFFER_SAMPLES - this->irRxDmaChannel->getDataLength();
  }
};
}  // namespace peripheral

#endif
