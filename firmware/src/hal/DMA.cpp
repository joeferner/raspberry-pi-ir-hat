#include "DMA.hpp"

namespace hal {
const void DMAChannel::setPeripheralRequest(dma::PeripheralRequest request) const {
  LL_DMA_SetPeriphRequest(this->dma->dma, (uint32_t)this->channel, (uint32_t)request);
}

const void DMAChannel::setDataTransferDirection(dma::TransferDirection direction) const {
  LL_DMA_SetDataTransferDirection(this->dma->dma, (uint32_t)this->channel, (uint32_t)direction);
}

const void DMAChannel::setChannelPriorityLevel(dma::Priority priority) const {
  LL_DMA_SetChannelPriorityLevel(this->dma->dma, (uint32_t)this->channel, (uint32_t)priority);
}

const void DMAChannel::setMode(dma::Mode mode) const {
  LL_DMA_SetMode(this->dma->dma, (uint32_t)this->channel, (uint32_t)mode);
}

const void DMAChannel::setPeripheralIncrementMode(dma::PeripheralIncrementMode mode) const {
  LL_DMA_SetPeriphIncMode(this->dma->dma, (uint32_t)this->channel, (uint32_t)mode);
}

const void DMAChannel::setMemoryIncrementMode(dma::MemoryIncrementMode mode) const {
  LL_DMA_SetMemoryIncMode(this->dma->dma, (uint32_t)this->channel, (uint32_t)mode);
}

const void DMAChannel::setPeripheralSize(dma::PeripheralSize size) const {
  LL_DMA_SetPeriphSize(this->dma->dma, (uint32_t)this->channel, (uint32_t)size);
}

const void DMAChannel::setMemorySize(dma::MemorySize size) const {
  LL_DMA_SetMemorySize(this->dma->dma, (uint32_t)this->channel, (uint32_t)size);
}

}  // namespace hal