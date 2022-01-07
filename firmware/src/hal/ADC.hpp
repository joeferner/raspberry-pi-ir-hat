#ifndef _ADC_HPP_
#define _ADC_HPP_

#include "main.h"

namespace hal {
namespace adc {
enum class ADCAddress : uint32_t
{ ADC1Address = ADC1_BASE };
}  // namespace adc

template <adc::ADCAddress TAddress>
class ADCHal {
 public:
  void enableClock(hal::Clocks& clocks) const {
    clocks.enableADCClock();
  }

  void enableEndOfConversionInterrupt() {
    LL_ADC_EnableIT_EOC(ADCAddress());
  }

  void enableEndOfSequenceInterrupt() {
    LL_ADC_EnableIT_EOS(ADCAddress());
  }

  void enableOverruneInterrupt() {
    LL_ADC_EnableIT_OVR(ADCAddress());
  }

  void enable() {
    LL_ADC_Enable(ADCAddress());
  }

  void startConversion() {
    LL_ADC_REG_StartConversion(ADCAddress());
  }

 private:
  ADC_TypeDef* ADCAddress() const {
    return reinterpret_cast<ADC_TypeDef*>(TAddress);
  }
};
}  // namespace hal

#endif
