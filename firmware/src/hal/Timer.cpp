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
  } else {
    assert_param(0);
  }
}

const void Timer::setAutoReload(uint32_t autoReload) const {
  LL_TIM_SetAutoReload(this->timer, autoReload);
}

const void Timer::setPrescaler(uint32_t prescaler) const {
  LL_TIM_SetPrescaler(this->timer, prescaler);
}

const void Timer::setRepetitionCounter(uint32_t repetitionCounter) const {
  if (IS_TIM_REPETITION_COUNTER_INSTANCE(this->timer)) {
    LL_TIM_SetRepetitionCounter(this->timer, repetitionCounter);
  } else {
    assert_param(0);
  }
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

const void Timer::enableOutputComparePreload(timer::Channel channel) const {
  LL_TIM_OC_EnablePreload(this->timer, (uint32_t)channel);
}

const void Timer::disableCaptureCompareChannel(timer::ChannelN channel) const {
  LL_TIM_CC_DisableChannel(this->timer, (uint32_t)channel);
}

const void Timer::setOutputCompareMode(timer::Channel channel, timer::OutputCompareMode mode) const {
  LL_TIM_OC_SetMode(this->timer, (uint32_t)channel, (uint32_t)mode);
}

const void Timer::setOutputComparePolarity(timer::ChannelN channel, timer::OutputComparePolarity polarity) const {
  LL_TIM_OC_SetPolarity(this->timer, (uint32_t)channel, (uint32_t)polarity);
}

const void Timer::setOutputCompareIdleState(timer::ChannelN channel, timer::OutputCompareIdleState idleState) const {
  LL_TIM_OC_SetIdleState(this->timer, (uint32_t)channel, (uint32_t)idleState);
}

const void Timer::setOutputCompareValue(timer::Channel channel, uint32_t value) const {
  switch (channel) {
    case timer::Channel::Channel1:
      LL_TIM_OC_SetCompareCH1(this->timer, value);
      break;
    case timer::Channel::Channel2:
      LL_TIM_OC_SetCompareCH2(this->timer, value);
      break;
    case timer::Channel::Channel3:
      LL_TIM_OC_SetCompareCH3(this->timer, value);
      break;
    case timer::Channel::Channel4:
      LL_TIM_OC_SetCompareCH4(this->timer, value);
      break;
    case timer::Channel::Channel5:
      LL_TIM_OC_SetCompareCH5(this->timer, value);
      break;
    default:
      assert_param(0);
      break;
  }
}

const void Timer::disableOutputCompareFast(timer::Channel channel) const {
  LL_TIM_OC_DisableFast(this->timer, (uint32_t)channel);
}
}  // namespace hal