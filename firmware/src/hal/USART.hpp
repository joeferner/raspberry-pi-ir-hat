#ifndef _USART_HPP_
#define _USART_HPP_

#include "RCCHal.hpp"
#include "config.h"

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
  USART_TypeDef *usart;

 public:
  USART(USART_TypeDef *usart) : usart(usart){};
  const void setDataWidth(usart::DataWidth dataWidth) const;
  const void setStopBits(usart::StopBits stopBits) const;
  const void setParity(usart::Parity parity) const;
  const void setOverSampling(usart::OverSampling oversampling) const;
  const usart::OverSampling getOverSampling() const;
  const void setTXFIFOThreshold(usart::FIFOThreshold threshold) const;
  const void setRXFIFOThreshold(usart::FIFOThreshold threshold) const;
  const void setTransferDirection(usart::TransferDirection direction) const;
  const void setHardwareFlowControl(usart::HardwardFlowControl flowControl) const;
  const void setPrescaler(usart::Prescaler prescaler) const;
  const usart::Prescaler getPrescalerValue() const;
  const void setBaudRate(const RCCHal *rcc, uint32_t baudRate) const;
  const void configAsyncMode() const;
  const void disableFIFO() const;
  const void enable() const;
  const bool isTransmitEnableAcknowledgeFlagSet() const;
  const bool isReceiveEnableAcknowledgeFlagSet() const;
};
}  // namespace hal

#endif
