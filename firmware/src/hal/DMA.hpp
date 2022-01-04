#ifndef _DMA_HPP_
#define _DMA_HPP_

#include "main.h"

namespace hal {
namespace dma {
enum class DMAAddress : uint32_t
{ DMA1Address = DMA1_BASE };

enum class Channel : uint32_t
{
  Channel1 = LL_DMA_CHANNEL_1,
  Channel2 = LL_DMA_CHANNEL_2,
  Channel3 = LL_DMA_CHANNEL_3,
  Channel4 = LL_DMA_CHANNEL_4,
  Channel5 = LL_DMA_CHANNEL_5,
#ifdef LL_DMA_CHANNEL_6
  Channel6 = LL_DMA_CHANNEL_6,
#endif
#ifdef LL_DMA_CHANNEL_7
  Channel7 = LL_DMA_CHANNEL_7
#endif
};

enum class PeripheralRequest : uint32_t
{
  MEM2MEM = LL_DMAMUX_REQ_MEM2MEM,
  GENERATOR0 = LL_DMAMUX_REQ_GENERATOR0,
  GENERATOR1 = LL_DMAMUX_REQ_GENERATOR1,
  GENERATOR2 = LL_DMAMUX_REQ_GENERATOR2,
  GENERATOR3 = LL_DMAMUX_REQ_GENERATOR3,
  RequestADC1 = LL_DMAMUX_REQ_ADC1,
#ifdef LL_DMAMUX_REQ_AES_IN
  AES_IN = LL_DMAMUX_REQ_AES_IN,
#endif
#ifdef LL_DMAMUX_REQ_AES_OUT
  AES_OUT = LL_DMAMUX_REQ_AES_OUT,
#endif
#ifdef LL_DMAMUX_REQ_DAC1_CH1
  DAC1_CH1 = LL_DMAMUX_REQ_DAC1_CH1,
#endif
#ifdef LL_DMAMUX_REQ_DAC1_CH2
  DAC1_CH2 = LL_DMAMUX_REQ_DAC1_CH2,
#endif
  I2C1_RX = LL_DMAMUX_REQ_I2C1_RX,
  I2C1_TX = LL_DMAMUX_REQ_I2C1_TX,
  I2C2_RX = LL_DMAMUX_REQ_I2C2_RX,
  I2C2_TX = LL_DMAMUX_REQ_I2C2_TX,
  LPUART1_RX = LL_DMAMUX_REQ_LPUART1_RX,
  LPUART1_TX = LL_DMAMUX_REQ_LPUART1_TX,
  SPI1_RX = LL_DMAMUX_REQ_SPI1_RX,
  SPI1_TX = LL_DMAMUX_REQ_SPI1_TX,
  SPI2_RX = LL_DMAMUX_REQ_SPI2_RX,
  SPI2_TX = LL_DMAMUX_REQ_SPI2_TX,
  TIM1_CH1 = LL_DMAMUX_REQ_TIM1_CH1,
  TIM1_CH2 = LL_DMAMUX_REQ_TIM1_CH2,
  TIM1_CH3 = LL_DMAMUX_REQ_TIM1_CH3,
  TIM1_CH4 = LL_DMAMUX_REQ_TIM1_CH4,
  TIM1_TRIG_COM = LL_DMAMUX_REQ_TIM1_TRIG_COM,
  TIM1_UP = LL_DMAMUX_REQ_TIM1_UP,
  TIM2_CH1 = LL_DMAMUX_REQ_TIM2_CH1,
  TIM2_CH2 = LL_DMAMUX_REQ_TIM2_CH2,
  TIM2_CH3 = LL_DMAMUX_REQ_TIM2_CH3,
  TIM2_CH4 = LL_DMAMUX_REQ_TIM2_CH4,
  TIM2_TRIG = LL_DMAMUX_REQ_TIM2_TRIG,
  TIM2_UP = LL_DMAMUX_REQ_TIM2_UP,
  TIM3_CH1 = LL_DMAMUX_REQ_TIM3_CH1,
  TIM3_CH2 = LL_DMAMUX_REQ_TIM3_CH2,
  TIM3_CH3 = LL_DMAMUX_REQ_TIM3_CH3,
  TIM3_CH4 = LL_DMAMUX_REQ_TIM3_CH4,
  TIM3_TRIG = LL_DMAMUX_REQ_TIM3_TRIG,
  TIM3_UP = LL_DMAMUX_REQ_TIM3_UP,
#ifdef LL_DMAMUX_REQ_TIM6_UP
  TIM6_UP = LL_DMAMUX_REQ_TIM6_UP,
#endif
#ifdef LL_DMAMUX_REQ_TIM7_UP
  TIM7_UP = LL_DMAMUX_REQ_TIM7_UP,
#endif
#ifdef LL_DMAMUX_REQ_TIM15_CH1
  TIM15_CH1 = LL_DMAMUX_REQ_TIM15_CH1,
#endif
#ifdef LL_DMAMUX_REQ_TIM15_CH2
  TIM15_CH2 = LL_DMAMUX_REQ_TIM15_CH2,
#endif
#ifdef LL_DMAMUX_REQ_TIM15_TRIG_COM
  TIM15_TRIG_COM = LL_DMAMUX_REQ_TIM15_TRIG_COM,
#endif
#ifdef LL_DMAMUX_REQ_TIM15_UP
  TIM15_UP = LL_DMAMUX_REQ_TIM15_UP,
#endif
  TIM16_CH1 = LL_DMAMUX_REQ_TIM16_CH1,
  TIM16_COM = LL_DMAMUX_REQ_TIM16_COM,
  TIM16_UP = LL_DMAMUX_REQ_TIM16_UP,
  TIM17_CH1 = LL_DMAMUX_REQ_TIM17_CH1,
  TIM17_COM = LL_DMAMUX_REQ_TIM17_COM,
  TIM17_UP = LL_DMAMUX_REQ_TIM17_UP,
  USART1_RX = LL_DMAMUX_REQ_USART1_RX,
  USART1_TX = LL_DMAMUX_REQ_USART1_TX,
  USART2_RX = LL_DMAMUX_REQ_USART2_RX,
  USART2_TX = LL_DMAMUX_REQ_USART2_TX,
#ifdef LL_DMAMUX_REQ_USART3_RX
  USART3_RX = LL_DMAMUX_REQ_USART3_RX,
#endif
#ifdef LL_DMAMUX_REQ_USART3_TX
  USART3_TX = LL_DMAMUX_REQ_USART3_TX,
#endif
#ifdef LL_DMAMUX_REQ_USART4_RX
  USART4_RX = LL_DMAMUX_REQ_USART4_RX,
#endif
#ifdef LL_DMAMUX_REQ_USART4_TX
  USART4_TX = LL_DMAMUX_REQ_USART4_TX,
#endif
#ifdef LL_DMAMUX_REQ_UCPD1_RX
  UCPD1_RX = LL_DMAMUX_REQ_UCPD1_RX,
#endif
#ifdef LL_DMAMUX_REQ_UCPD1_TX
  UCPD1_TX = LL_DMAMUX_REQ_UCPD1_TX,
#endif
#ifdef LL_DMAMUX_REQ_UCPD2_RX
  UCPD2_RX = LL_DMAMUX_REQ_UCPD2_RX,
#endif
#ifdef LL_DMAMUX_REQ_UCPD2_TX
  UCPD2_TX = LL_DMAMUX_REQ_UCPD2_TX,
#endif
};

enum class TransferDirection : uint32_t
{
  PeripheralToMemory = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
  MemoryToPeripheral = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
  MemoryToMemory = LL_DMA_DIRECTION_MEMORY_TO_MEMORY
};

enum class Priority : uint32_t
{
  Low = LL_DMA_PRIORITY_LOW,
  Medium = LL_DMA_PRIORITY_MEDIUM,
  High = LL_DMA_PRIORITY_HIGH,
  VeryHigh = LL_DMA_PRIORITY_VERYHIGH
};

enum class Mode : uint32_t
{
  Normal = LL_DMA_MODE_NORMAL,
  Circular = LL_DMA_MODE_CIRCULAR
};

enum class PeripheralIncrementMode : uint32_t
{
  Increment = LL_DMA_PERIPH_INCREMENT,
  NoIncrement = LL_DMA_PERIPH_NOINCREMENT
};

enum class MemoryIncrementMode : uint32_t
{
  Increment = LL_DMA_MEMORY_INCREMENT,
  NoIncrement = LL_DMA_MEMORY_NOINCREMENT
};

enum class PeripheralSize : uint32_t
{
  Byte = LL_DMA_PDATAALIGN_BYTE,
  HalfWord = LL_DMA_PDATAALIGN_HALFWORD,
  Word = LL_DMA_PDATAALIGN_WORD
};

enum class MemorySize : uint32_t
{
  Byte = LL_DMA_MDATAALIGN_BYTE,
  HalfWord = LL_DMA_MDATAALIGN_HALFWORD,
  Word = LL_DMA_MDATAALIGN_WORD
};
}  // namespace dma

template <dma::DMAAddress TAddress>
class DMA {};

template <dma::DMAAddress TAddress, dma::Channel TChannel>
class DMAChannel {
 private:
  DMA_TypeDef* DMAAddress() const {
    return reinterpret_cast<DMA_TypeDef*>(TAddress);
  }

  uint32_t channel() const {
    return (uint32_t)TChannel;
  }

 public:
  const void setPeripheralRequest(dma::PeripheralRequest request) const {
    LL_DMA_SetPeriphRequest(DMAAddress(), channel(), (uint32_t)request);
  }

  const void setDataTransferDirection(dma::TransferDirection direction) const {
    LL_DMA_SetDataTransferDirection(DMAAddress(), channel(), (uint32_t)direction);
  }

  const void setChannelPriorityLevel(dma::Priority priority) const {
    LL_DMA_SetChannelPriorityLevel(DMAAddress(), channel(), (uint32_t)priority);
  }

  const void setMode(dma::Mode mode) const {
    LL_DMA_SetMode(DMAAddress(), channel(), (uint32_t)mode);
  }

  const void setPeripheralIncrementMode(dma::PeripheralIncrementMode mode) const {
    LL_DMA_SetPeriphIncMode(DMAAddress(), channel(), (uint32_t)mode);
  }

  const void setMemoryIncrementMode(dma::MemoryIncrementMode mode) const {
    LL_DMA_SetMemoryIncMode(DMAAddress(), channel(), (uint32_t)mode);
  }

  const void setPeripheralSize(dma::PeripheralSize size) const {
    LL_DMA_SetPeriphSize(DMAAddress(), channel(), (uint32_t)size);
  }

  const void setMemorySize(dma::MemorySize size) const {
    LL_DMA_SetMemorySize(DMAAddress(), channel(), (uint32_t)size);
  }

  const void enableTransferCompleteInterrupt() const {
    LL_DMA_EnableIT_TC(DMAAddress(), channel());
  }

  const void enableTransferErrorInterrupt() const {
    LL_DMA_EnableIT_TE(DMAAddress(), channel());
  }

  const void disable() const {
    LL_DMA_DisableChannel(DMAAddress(), channel());
  }

  const void enable() const {
    LL_DMA_EnableChannel(DMAAddress(), channel());
  }

  const void clearGlobalInterruptFlag() const {
    switch (TChannel) {
      case dma::Channel::Channel1:
        LL_DMA_ClearFlag_GI1(DMAAddress());
        break;
      case dma::Channel::Channel2:
        LL_DMA_ClearFlag_GI2(DMAAddress());
        break;
      case dma::Channel::Channel3:
        LL_DMA_ClearFlag_GI3(DMAAddress());
        break;
      case dma::Channel::Channel4:
        LL_DMA_ClearFlag_GI4(DMAAddress());
        break;
      case dma::Channel::Channel5:
        LL_DMA_ClearFlag_GI5(DMAAddress());
        break;
#ifdef LL_DMA_CHANNEL_6
      case dma::Channel::Channel6:
        LL_DMA_ClearFlag_GI6(DMAAddress());
        break;
#endif
#ifdef LL_DMA_CHANNEL_7
      case dma::Channel::Channel7:
        LL_DMA_ClearFlag_GI7(DMAAddress());
        break;
#endif
      default:
        assert_param(0);
    }
  }

  const void setDataLength(uint32_t dataLength) const {
    LL_DMA_SetDataLength(DMAAddress(), channel(), dataLength);
  }

  const uint32_t getDataLength() const {
    return LL_DMA_GetDataLength(DMAAddress(), channel());
  }

  const void setPeripheralAddress(const void* p) const {
    LL_DMA_SetPeriphAddress(DMAAddress(), channel(), (uint32_t)p);
  }

  const void setMemoryAddress(const void* p) const {
    LL_DMA_SetMemoryAddress(DMAAddress(), channel(), (uint32_t)p);
  }
};
}  // namespace hal

#endif
