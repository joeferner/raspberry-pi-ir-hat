#ifndef _CURRENT_SENSOR_HPP_
#define _CURRENT_SENSOR_HPP_

#include <stdlib.h>

#include "BothUSARTWriter.hpp"
#include "hal/ADC.hpp"
#include "hal/Clocks.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "main.h"

namespace peripheral {
enum class CurrentReadRank
{
  Reference,
  Current1,
  Current0,
  Unknown,
};

enum class CurrentSensorInput
{
  Reference,
  Current1,
  Current0,
};

class CurrentSensor {
 private:
  hal::ADCHal<hal::adc::ADCAddress::ADC1Address>* adc;

  uint16_t currentReference_mV;
  uint16_t current_0_mV;
  uint16_t current_0_localMax_mV;
  uint16_t current_1_mV;
  uint16_t current_1_localMax_mV;
  uint16_t sequenceCount;
  CurrentReadRank currentReadRank;
  volatile bool endOfConversion;
  volatile bool endOfSequence;

 public:
  CurrentSensor(hal::ADCHal<hal::adc::ADCAddress::ADC1Address>* adc) : adc(adc) {
  }

  void initialize(
      hal::Clocks& clocks,
      hal::NVICHal& nvic,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin0>& currentRefPin,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin4>& current0Pin,
      hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin5>& current1Pin);

  void loop();

  void handleInterrupt();

  const uint16_t get(CurrentSensorInput input) const;

 private:
  void handleEndOfConversion();

  void handleEndOfSequence();

  void handleOverrunError();
};
}  // namespace peripheral

#endif
