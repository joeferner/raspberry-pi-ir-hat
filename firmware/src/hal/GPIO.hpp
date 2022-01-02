#ifndef _GPIO_HPP_
#define _GPIO_HPP_

#include "config.h"

namespace hal {

namespace gpio {
enum class Mode : uint32_t {
  Input = LL_GPIO_MODE_INPUT,
  Output = LL_GPIO_MODE_OUTPUT,
  Alternate = LL_GPIO_MODE_ALTERNATE,
  Analog = LL_GPIO_MODE_ANALOG
};

enum class Pull : uint32_t { None = LL_GPIO_PULL_NO, Up = LL_GPIO_PULL_UP, Down = LL_GPIO_PULL_DOWN };

enum class Alternate : uint32_t {
  Alt0 = LL_GPIO_AF_0,
  Alt1 = LL_GPIO_AF_1,
  Alt2 = LL_GPIO_AF_2,
  Alt3 = LL_GPIO_AF_3,
  Alt4 = LL_GPIO_AF_4,
  Alt5 = LL_GPIO_AF_5,
  Alt6 = LL_GPIO_AF_6,
  Alt7 = LL_GPIO_AF_7,
};

enum class Speed : uint32_t {
  Low = LL_GPIO_SPEED_FREQ_LOW,
  Medium = LL_GPIO_SPEED_FREQ_MEDIUM,
  High = LL_GPIO_SPEED_FREQ_HIGH,
  VeryHigh = LL_GPIO_SPEED_FREQ_VERY_HIGH
};

enum class OutputType : uint32_t { PushPull = LL_GPIO_OUTPUT_PUSHPULL, OpenDrain = LL_GPIO_OUTPUT_OPENDRAIN };
}  // namespace gpio

class GPIO {
 private:
  GPIO_TypeDef* port;
  unsigned int pin;

 public:
  GPIO(GPIO_TypeDef* port, int pin) : port(port), pin(pin) {}

  /**
   * @brief Set the pin mode (input, output, etc)
   */
  const void setMode(gpio::Mode mode) const;

  /**
   * @brief Set the pin to have no pull up or pull down resistor
   */
  const void setPull(gpio::Pull pull) const;

  /**
   * @brief Set the pin alternate mode
   */
  const void setAlternate(gpio::Alternate alternate) const;

  /**
   * @brief set pin to low
   */
  const void resetOutputPin() const;

  /**
   * @brief Set pin speed
   */
  const void setSpeed(gpio::Speed speed) const;

  /**
   * @brief Set pin output type
   */
  const void setOutputType(gpio::OutputType outputType) const;
};

}  // namespace hal

#endif
