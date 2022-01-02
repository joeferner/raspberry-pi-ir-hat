#include "Timer.hpp"

namespace hal {
const void Timer::setCounterMode(timer::CounterMode mode) const {
  if (IS_TIM_COUNTER_MODE_SELECT_INSTANCE(this->timer)) {
    LL_TIM_SetCounterMode(this->timer, (uint32_t)mode);
  } else {
    assert_param(0);
  }
}

const void Timer::setClockDivision(timer::ClockDivision clockDivision) const {
  if (IS_TIM_CLOCK_DIVISION_INSTANCE(this->timer)) {
    LL_TIM_SetClockDivision(this->timer, (uint32_t)clockDivision);
  }
}

const void Timer::setAutoReload(uint32_t autoReload) const {
  LL_TIM_SetAutoReload(this->timer, autoReload);
}

const void Timer::setPrescaler(uint32_t prescaler) const {
  LL_TIM_SetPrescaler(this->timer, prescaler);
}

const void Timer::disableAutoReloadPreload() const {
  LL_TIM_DisableARRPreload(this->timer);
}

const void Timer::setTriggerOutput(timer::TriggerOutput triggerOutput) const {
  LL_TIM_SetTriggerOutput(this->timer, (uint32_t)triggerOutput);
}

const void Timer::disableMasterSlaveMode() const {
  LL_TIM_DisableMasterSlaveMode(this->timer);
}

const void Timer::setInputCaptureActiveInput(timer::Channel channel, timer::InputCaptureActiveInput activeInput) const {
  LL_TIM_IC_SetActiveInput(this->timer, (uint32_t)channel, (uint32_t)activeInput);
}

const void Timer::setInputCapturePrescaler(timer::Channel channel, timer::InputCapturePrescaler prescaler) const {
  LL_TIM_IC_SetPrescaler(this->timer, (uint32_t)channel, (uint32_t)prescaler);
}

const void Timer::setInputCaptureFilter(timer::Channel channel, timer::InputCaptureFilter filter) const {
  LL_TIM_IC_SetFilter(this->timer, (uint32_t)channel, (uint32_t)filter);
}

const void Timer::setInputCapturePolarity(timer::Channel channel, timer::InputCapturePolarity polarity) const {
  LL_TIM_IC_SetPolarity(this->timer, (uint32_t)channel, (uint32_t)polarity);
}
}  // namespace hal