#ifndef _HAL_USART_HPP_
#define _HAL_USART_HPP_

#include "RCC.hpp"
#include "main.h"

namespace hal {
namespace usart {
enum class FIFOThreshold : uint32_t
{
  Threshold1_8 = LL_USART_FIFOTHRESHOLD_1_8,
  Threshold1_4 = LL_USART_FIFOTHRESHOLD_1_4,
  Threshold1_2 = LL_USART_FIFOTHRESHOLD_1_2,
  Threshold3_4 = LL_USART_FIFOTHRESHOLD_3_4,
  Threshold7_8 = LL_USART_FIFOTHRESHOLD_7_8,
  Threshold8_8 = LL_USART_FIFOTHRESHOLD_8_8
};

enum class DataWidth : uint32_t
{
  DataWidth7 = LL_USART_DATAWIDTH_7B,
  DataWidth8 = LL_USART_DATAWIDTH_8B,
  DataWidth9 = LL_USART_DATAWIDTH_9B
};

enum class StopBits : uint32_t
{
  StopBits0_5 = LL_USART_STOPBITS_0_5,
  StopBits1 = LL_USART_STOPBITS_1,
  StopBits1_5 = LL_USART_STOPBITS_1_5,
  StopBits2 = LL_USART_STOPBITS_2
};

enum class Parity : uint32_t
{
  None = LL_USART_PARITY_NONE,
  Even = LL_USART_PARITY_EVEN,
  Odd = LL_USART_PARITY_ODD
};

enum class OverSampling : uint32_t
{
  OverSampling16 = LL_USART_OVERSAMPLING_16,
  OverSampling8 = LL_USART_OVERSAMPLING_8
};

enum class TransferDirection : uint32_t
{
  None = LL_USART_DIRECTION_NONE,
  Rx = LL_USART_DIRECTION_RX,
  Tx = LL_USART_DIRECTION_TX,
  TxRx = LL_USART_DIRECTION_TX_RX
};

enum class HardwardFlowControl : uint32_t
{
  None = LL_USART_HWCONTROL_NONE,
  RTS = LL_USART_HWCONTROL_RTS,
  CTS = LL_USART_HWCONTROL_CTS,
  RTS_CTS = LL_USART_HWCONTROL_RTS_CTS,
};

enum class Prescaler : uint32_t
{
  DIV_1 = LL_USART_PRESCALER_DIV1,
  DIV_2 = LL_USART_PRESCALER_DIV2,
  DIV_4 = LL_USART_PRESCALER_DIV4,
  DIV_6 = LL_USART_PRESCALER_DIV6,
  DIV_8 = LL_USART_PRESCALER_DIV8,
  DIV_10 = LL_USART_PRESCALER_DIV10,
  DIV_12 = LL_USART_PRESCALER_DIV12,
  DIV_16 = LL_USART_PRESCALER_DIV16,
  DIV_32 = LL_USART_PRESCALER_DIV32,
  DIV_64 = LL_USART_PRESCALER_DIV64,
  DIV_128 = LL_USART_PRESCALER_DIV128,
  DIV_256 = LL_USART_PRESCALER_DIV256
};
}  // namespace usart

class USART {
 private:
  USART_TypeDef* usart;

 public:
  USART(USART_TypeDef* usart) : usart(usart){};
  const void setDataWidth(usart::DataWidth dataWidth) const {
    LL_USART_SetDataWidth(this->usart, (uint32_t)dataWidth);
  }

  const void setStopBits(usart::StopBits stopBits) const {
    LL_USART_SetStopBitsLength(this->usart, (uint32_t)stopBits);
  }

  const void setParity(usart::Parity parity) const {
    LL_USART_SetParity(this->usart, (uint32_t)parity);
  }

  const void setOverSampling(usart::OverSampling oversampling) const {
    LL_USART_SetOverSampling(this->usart, (uint32_t)oversampling);
  }

  const usart::OverSampling getOverSampling() const {
    return (usart::OverSampling)LL_USART_GetOverSampling(this->usart);
  }

  const void setTXFIFOThreshold(usart::FIFOThreshold threshold) const {
    LL_USART_SetTXFIFOThreshold(this->usart, (uint32_t)threshold);
  }

  const void setRXFIFOThreshold(usart::FIFOThreshold threshold) const {
    LL_USART_SetRXFIFOThreshold(this->usart, (uint32_t)threshold);
  }

  const void setTransferDirection(usart::TransferDirection direction) const {
    LL_USART_SetTransferDirection(this->usart, (uint32_t)direction);
  }

  const void setHardwareFlowControl(usart::HardwardFlowControl flowControl) const {
    LL_USART_SetHWFlowCtrl(this->usart, (uint32_t)flowControl);
  }

  const void setPrescaler(usart::Prescaler prescaler) const {
    LL_USART_SetPrescaler(this->usart, (uint32_t)prescaler);
  }

  const usart::Prescaler getPrescalerValue() const {
    return (usart::Prescaler)LL_USART_GetPrescaler(this->usart);
  }

  const void setBaudRate(const RCCHal* rcc, uint32_t baudRate) const {
    uint32_t clock;
    if (this->usart == USART1) {
      clock = rcc->getUSART1ClockFrequency();
    } else if (this->usart == USART2) {
#if defined(RCC_CCIPR_USART2SEL)
      clock = rcc->getUSART2ClockFrequency();
#else
      clock = rcc->getPCLK1Frequency();
#endif
    }
#if defined(USART3)
    else if (this->usart == USART3) {
#if defined(RCC_CCIPR_USART3SEL)
      clock = rcc->getUSART3ClockFrequency();
#else
      clock = rcc->getPCLK1Frequency();
#endif
    }
#endif /* USART3 */
#if defined(USART4)
    else if (this->usart == USART4) {
#if defined(RCC_CCIPR_USART4SEL)
      clock = rcc->getUSART4ClockFrequency();
#else
      clock = rcc->getPCLK1Frequency();
#endif /* RCC_CCIPR_USART4SEL */
    }
#endif /* USART4 */
#if defined(USART5)
    else if (this->usart == USART5) {
      clock = rcc->getPCLK1Frequency();
    }
#endif /* USART5 */
#if defined(USART6)
    else if (this->usart == USART6) {
      clock = rcc->getPCLK1Frequency();
    }
#endif /* USART6 */
    else {
      clock = 0;
      assert_param(0);
    }

    LL_USART_SetBaudRate(this->usart, clock, (uint32_t)this->getPrescalerValue(), (uint32_t)this->getOverSampling(),
                         baudRate);
  }

  const void configAsyncMode() const {
    LL_USART_ConfigAsyncMode(this->usart);
  }

  const void disableFIFO() const {
    LL_USART_DisableFIFO(this->usart);
  }

  const void enable() const {
    LL_USART_Enable(this->usart);
    while (!this->isTransmitEnableAcknowledgeFlagSet() || !this->isReceiveEnableAcknowledgeFlagSet()) {
    }
  }

  const bool isTransmitEnableAcknowledgeFlagSet() const {
    return LL_USART_IsActiveFlag_TEACK(this->usart);
  }

  const bool isReceiveEnableAcknowledgeFlagSet() const {
    return LL_USART_IsActiveFlag_REACK(this->usart);
  }

  const void enableRxNotEmptyInterrupt() const {
    LL_USART_EnableIT_RXNE_RXFNE(this->usart);
  }

  const void enableTxEmptyInterrupt() const {
    LL_USART_EnableIT_TXE_TXFNF(this->usart);
  }

  const void enableErrorInterrupt() const {
    LL_USART_EnableIT_ERROR(this->usart);
  }

  const bool isTxDataRegisterEmptyFlagSet() const {
    return LL_USART_IsActiveFlag_TXE_TXFNF(this->usart);
  }

  const bool isTransmissionCompleteFlagSet() const {
    return LL_USART_IsActiveFlag_TC(this->usart);
  }

  const void clearTransmissionCompleteFlag() const {
    LL_USART_ClearFlag_TC(this->usart);
  }

  const bool isRxNotEmptyFlagSet() const {
    return LL_USART_IsActiveFlag_RXNE_RXFNE(this->usart);
  }

  const bool isErrorFlagSet() const {
    return LL_USART_IsActiveFlag_NE(this->usart);
  }

  const void disableTxEmptyInterrupt() const {
    LL_USART_DisableIT_TXE_TXFNF(this->usart);
  }

  const void transmitData8(uint8_t b) const {
    LL_USART_TransmitData8(this->usart, b);
  }

  const uint8_t receiveData8() const {
    return LL_USART_ReceiveData8(this->usart);
  }
};
}  // namespace hal

#endif
