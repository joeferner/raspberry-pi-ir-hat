#ifndef _GPIO_HPP_
#define _GPIO_HPP_

#include "main.h"

namespace hal {

namespace gpio {
enum class GPIOAddress : uint32_t
{
  GPIOAAddress = GPIOA_BASE,
  GPIOBAddress = GPIOB_BASE,
  GPIOFAddress = GPIOF_BASE
};

enum class GPIOPin : uint32_t
{
  Pin0 = LL_GPIO_PIN_0,
  Pin1 = LL_GPIO_PIN_1,
  Pin2 = LL_GPIO_PIN_2,
  Pin3 = LL_GPIO_PIN_3,
  Pin4 = LL_GPIO_PIN_4,
  Pin5 = LL_GPIO_PIN_5,
  Pin6 = LL_GPIO_PIN_6,
  Pin7 = LL_GPIO_PIN_7,
  Pin8 = LL_GPIO_PIN_8,
  Pin9 = LL_GPIO_PIN_9,
  Pin10 = LL_GPIO_PIN_10,
  Pin11 = LL_GPIO_PIN_11,
  Pin12 = LL_GPIO_PIN_12,
  Pin13 = LL_GPIO_PIN_13,
  Pin14 = LL_GPIO_PIN_14,
  Pin15 = LL_GPIO_PIN_15
};

enum class Mode : uint32_t
{
  Input = LL_GPIO_MODE_INPUT,
  Output = LL_GPIO_MODE_OUTPUT,
  Alternate = LL_GPIO_MODE_ALTERNATE,
  Analog = LL_GPIO_MODE_ANALOG
};

enum class Pull : uint32_t
{
  None = LL_GPIO_PULL_NO,
  Up = LL_GPIO_PULL_UP,
  Down = LL_GPIO_PULL_DOWN
};

enum class Alternate : uint32_t
{
  Alt0 = LL_GPIO_AF_0,
  Alt1 = LL_GPIO_AF_1,
  Alt2 = LL_GPIO_AF_2,
  Alt3 = LL_GPIO_AF_3,
  Alt4 = LL_GPIO_AF_4,
  Alt5 = LL_GPIO_AF_5,
  Alt6 = LL_GPIO_AF_6,
  Alt7 = LL_GPIO_AF_7,
};

enum class Speed : uint32_t
{
  Low = LL_GPIO_SPEED_FREQ_LOW,
  Medium = LL_GPIO_SPEED_FREQ_MEDIUM,
  High = LL_GPIO_SPEED_FREQ_HIGH,
  VeryHigh = LL_GPIO_SPEED_FREQ_VERY_HIGH
};

enum class OutputType : uint32_t
{
  PushPull = LL_GPIO_OUTPUT_PUSHPULL,
  OpenDrain = LL_GPIO_OUTPUT_OPENDRAIN
};
}  // namespace gpio

template <gpio::GPIOAddress TAddress, gpio::GPIOPin TPin>
class GPIO {
 private:
  GPIO_TypeDef* GPIOPort() const {
    return reinterpret_cast<GPIO_TypeDef*>(TAddress);
  }

  uint32_t GPIOPin() const {
    return (uint32_t)TPin;
  }

 public:
  /**
   * @brief Set the pin mode (input, output, etc)
   */
  void setMode(gpio::Mode mode) const {
    LL_GPIO_SetPinMode(GPIOPort(), GPIOPin(), (uint32_t)mode);
  }

  /**
   * @brief Set the pin to have no pull up or pull down resistor
   */
  void setPull(gpio::Pull pull) const {
    LL_GPIO_SetPinPull(GPIOPort(), GPIOPin(), (uint32_t)pull);
  }

  /**
   * @brief Set the pin alternate mode
   */
  void setAlternate(gpio::Alternate alternate) const {
    if (GPIOPin() < LL_GPIO_PIN_8) {
      LL_GPIO_SetAFPin_0_7(GPIOPort(), GPIOPin(), (uint32_t)alternate);
    } else {
      LL_GPIO_SetAFPin_8_15(GPIOPort(), GPIOPin(), (uint32_t)alternate);
    }
  }

  /**
   * @brief set pin to low
   */
  void resetOutputPin() const {
    LL_GPIO_ResetOutputPin(GPIOPort(), GPIOPin());
  }

  /**
   * @brief set pin high
   */
  void setOutputPin() {
    LL_GPIO_SetOutputPin(GPIOPort(), GPIOPin());
  }

  /**
   * @brief Set pin speed
   */
  void setSpeed(gpio::Speed speed) const {
    LL_GPIO_SetPinSpeed(GPIOPort(), GPIOPin(), (uint32_t)speed);
  }

  /**
   * @brief Set pin output type
   */
  void setOutputType(gpio::OutputType outputType) const {
    LL_GPIO_SetPinOutputType(GPIOPort(), GPIOPin(), (uint32_t)outputType);
  }

  void enableClock(hal::Clocks& clocks) const {
    switch (TAddress) {
      case gpio::GPIOAddress::GPIOAAddress:
        clocks.enableGPIOAClock();
        break;
      case gpio::GPIOAddress::GPIOBAddress:
        clocks.enableGPIOBClock();
        break;
      case gpio::GPIOAddress::GPIOFAddress:
        clocks.enableGPIOFClock();
        break;
      default:
        assert_param(0);
    }
  }
};

}  // namespace hal

#endif
