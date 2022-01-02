#include "USART.hpp"

namespace hal {

const void USART::setDataWidth(usart::DataWidth dataWidth) const {
  LL_USART_SetDataWidth(this->usart, (uint32_t)dataWidth);
}

const void USART::setStopBits(usart::StopBits stopBits) const {
  LL_USART_SetStopBitsLength(this->usart, (uint32_t)stopBits);
}

const void USART::setParity(usart::Parity parity) const {
  LL_USART_SetParity(this->usart, (uint32_t)parity);
}

const void USART::setOverSampling(usart::OverSampling oversampling) const {
  LL_USART_SetOverSampling(this->usart, (uint32_t)oversampling);
}

const usart::OverSampling USART::getOverSampling() const {
  return (usart::OverSampling)LL_USART_GetOverSampling(this->usart);
}

const void USART::setTransferDirection(usart::TransferDirection direction) const {
  LL_USART_SetTransferDirection(this->usart, (uint32_t)direction);
}

const void USART::setHardwareFlowControl(usart::HardwardFlowControl flowControl) const {
  LL_USART_SetHWFlowCtrl(this->usart, (uint32_t)flowControl);
}

const void USART::setPrescaler(usart::Prescaler prescaler) const {
  LL_USART_SetPrescaler(this->usart, (uint32_t)prescaler);
}

const usart::Prescaler USART::getPrescalerValue() const {
  return (usart::Prescaler)LL_USART_GetPrescaler(this->usart);
}

const void USART::setBaudRate(const RCCHal *rcc, uint32_t baudRate) const {
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

const void USART::disableFIFO() const {
  LL_USART_DisableFIFO(this->usart);
}

const void USART::configAsyncMode() const {
  LL_USART_ConfigAsyncMode(this->usart);
}

const void USART::setTXFIFOThreshold(usart::FIFOThreshold threshold) const {
  LL_USART_SetTXFIFOThreshold(this->usart, (uint32_t)threshold);
};

const void USART::setRXFIFOThreshold(usart::FIFOThreshold threshold) const {
  LL_USART_SetRXFIFOThreshold(this->usart, (uint32_t)threshold);
};

const void USART::enable() const {
  LL_USART_Enable(this->usart);
  while (!this->isTransmitEnableAcknowledgeFlagSet() || !this->isReceiveEnableAcknowledgeFlagSet()) {
  }
}

const bool USART::isTransmitEnableAcknowledgeFlagSet() const {
  return LL_USART_IsActiveFlag_TEACK(this->usart);
}

const bool USART::isReceiveEnableAcknowledgeFlagSet() const {
  return LL_USART_IsActiveFlag_REACK(this->usart);
}

}  // namespace hal