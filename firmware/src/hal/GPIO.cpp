#include "GPIO.hpp"

namespace hal {

const void GPIO::setMode(gpio::Mode mode) const { LL_GPIO_SetPinMode(this->port, this->pin, (uint32_t)mode); }

const void GPIO::setPull(gpio::Pull pull) const { LL_GPIO_SetPinPull(this->port, this->pin, (uint32_t)pull); }

const void GPIO::setAlternate(gpio::Alternate alternate) const {
  if (this->pin < LL_GPIO_PIN_8) {
    LL_GPIO_SetAFPin_0_7(this->port, this->pin, (uint32_t)alternate);
  } else {
    LL_GPIO_SetAFPin_8_15(this->port, this->pin, (uint32_t)alternate);
  }
}

const void GPIO::resetOutputPin() const { LL_GPIO_ResetOutputPin(this->port, this->pin); }

const void GPIO::setSpeed(gpio::Speed speed) const { LL_GPIO_SetPinSpeed(this->port, this->pin, (uint32_t)speed); }

const void GPIO::setOutputType(gpio::OutputType outputType) const {
  LL_GPIO_SetPinOutputType(this->port, this->pin, (uint32_t)outputType);
}

}  // namespace hal