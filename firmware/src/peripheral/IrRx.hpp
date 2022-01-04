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
    this->irRxDmaChannel->setDataLength(sizeof(this->buffer));
    this->irRxDmaChannel->setPeripheralAddress((const char*)irRxTimer.getCaptureCompare1RegisterAddress());
    this->irRxDmaChannel->setMemoryAddress(this->buffer);
    this->irRxDmaChannel->setMemorySize(hal::dma::MemorySize::HalfWord);
    memset(this->buffer, 0, sizeof(this->buffer));
    this->irRxDmaChannel->enable();
    this->readIndex = this->getDmaPosition();

    irRxTimer.enableCaptureCompareInterrupt(hal::timer::Channel::Channel1);
    irRxTimer.enableCaptureCompareDMARequest(hal::timer::Channel::Channel1);
    irRxTimer.enableChannel(hal::timer::ChannelN::Channel1);
    irRxTimer.enableCounter();
  }

 private:
  const size_t getDmaPosition() const {
    return sizeof(this->buffer) - this->irRxDmaChannel->getDataLength();
  }
};
}  // namespace peripheral

#endif
