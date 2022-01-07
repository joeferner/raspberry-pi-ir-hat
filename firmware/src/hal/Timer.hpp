#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "main.h"

namespace hal {
namespace timer {
enum class TimerAddress : uint32_t
{
  TIM1Address = TIM1_BASE,
  TIM2Address = TIM2_BASE,
  TIM3Address = TIM3_BASE,
  TIM14Address = TIM14_BASE,
  TIM16Address = TIM16_BASE,
  TIM17Address = TIM17_BASE,
};

enum class CounterMode : uint32_t
{
  Up = LL_TIM_COUNTERMODE_UP,
  Down = LL_TIM_COUNTERMODE_DOWN,
  CenterDown = LL_TIM_COUNTERMODE_CENTER_DOWN,
  CenterUp = LL_TIM_COUNTERMODE_CENTER_UP,
  CenterUpDown = LL_TIM_COUNTERMODE_CENTER_UP_DOWN
};

enum class ClockDivision : uint32_t
{
  DIV_1 = LL_TIM_CLOCKDIVISION_DIV1,
  DIV_2 = LL_TIM_CLOCKDIVISION_DIV2,
  DIV_4 = LL_TIM_CLOCKDIVISION_DIV4
};

enum class TriggerOutput : uint32_t
{
  Reset = LL_TIM_TRGO_RESET,
  Enable = LL_TIM_TRGO_ENABLE,
  Update = LL_TIM_TRGO_UPDATE,
  CC1IF = LL_TIM_TRGO_CC1IF,
  OC1REF = LL_TIM_TRGO_OC1REF,
  OC2REF = LL_TIM_TRGO_OC2REF,
  OC3REF = LL_TIM_TRGO_OC3REF,
  OC4REF = LL_TIM_TRGO_OC4REF
};

enum class Channel : uint32_t
{
  Channel1 = LL_TIM_CHANNEL_CH1,
  Channel2 = LL_TIM_CHANNEL_CH2,
  Channel3 = LL_TIM_CHANNEL_CH3,
  Channel4 = LL_TIM_CHANNEL_CH4,
  Channel5 = LL_TIM_CHANNEL_CH5
};

enum class ChannelN : uint32_t
{
  Channel1 = LL_TIM_CHANNEL_CH1,
  Channel1N = LL_TIM_CHANNEL_CH1N,
  Channel2 = LL_TIM_CHANNEL_CH2,
  Channel2N = LL_TIM_CHANNEL_CH2N,
  Channel3 = LL_TIM_CHANNEL_CH3,
  Channel3N = LL_TIM_CHANNEL_CH3N,
  Channel4 = LL_TIM_CHANNEL_CH4,
  Channel5 = LL_TIM_CHANNEL_CH5
};

enum class InputCaptureActiveInput : uint32_t
{
  DirectTI = LL_TIM_ACTIVEINPUT_DIRECTTI,
  IndirectTI = LL_TIM_ACTIVEINPUT_INDIRECTTI,
  TRC = LL_TIM_ACTIVEINPUT_TRC
};

enum class InputCapturePrescaler : uint32_t
{
  DIV_1 = LL_TIM_ICPSC_DIV1,
  DIV_2 = LL_TIM_ICPSC_DIV2,
  DIV_4 = LL_TIM_ICPSC_DIV4,
  DIV_8 = LL_TIM_ICPSC_DIV8
};

enum class InputCaptureFilter : uint32_t
{
  FDIV1 = LL_TIM_IC_FILTER_FDIV1,
  FDIV1_N2 = LL_TIM_IC_FILTER_FDIV1_N2,
  FDIV1_N4 = LL_TIM_IC_FILTER_FDIV1_N4,
  FDIV1_N8 = LL_TIM_IC_FILTER_FDIV1_N8,
  FDIV2_N6 = LL_TIM_IC_FILTER_FDIV2_N6,
  FDIV2_N8 = LL_TIM_IC_FILTER_FDIV2_N8,
  FDIV4_N6 = LL_TIM_IC_FILTER_FDIV4_N6,
  FDIV4_N8 = LL_TIM_IC_FILTER_FDIV4_N8,
  FDIV8_N6 = LL_TIM_IC_FILTER_FDIV8_N6,
  FDIV8_N8 = LL_TIM_IC_FILTER_FDIV8_N8,
  FDIV16_N5 = LL_TIM_IC_FILTER_FDIV16_N5,
  FDIV16_N6 = LL_TIM_IC_FILTER_FDIV16_N6,
  FDIV16_N8 = LL_TIM_IC_FILTER_FDIV16_N8,
  FDIV32_N5 = LL_TIM_IC_FILTER_FDIV32_N5,
  FDIV32_N6 = LL_TIM_IC_FILTER_FDIV32_N6,
  FDIV32_N8 = LL_TIM_IC_FILTER_FDIV32_N8
};

enum class InputCapturePolarity : uint32_t
{
  Rising = LL_TIM_IC_POLARITY_RISING,
  Falling = LL_TIM_IC_POLARITY_FALLING,
  BothEdges = LL_TIM_IC_POLARITY_BOTHEDGE
};

enum class OutputCompareMode : uint32_t
{
  Frozen = LL_TIM_OCMODE_FROZEN,
  Active = LL_TIM_OCMODE_ACTIVE,
  Inactive = LL_TIM_OCMODE_INACTIVE,
  Toggle = LL_TIM_OCMODE_TOGGLE,
  ForcedInactive = LL_TIM_OCMODE_FORCED_INACTIVE,
  ForcedActive = LL_TIM_OCMODE_FORCED_ACTIVE,
  PWM1 = LL_TIM_OCMODE_PWM1,
  PWM2 = LL_TIM_OCMODE_PWM2,
  RetrigOPM1 = LL_TIM_OCMODE_RETRIG_OPM1,
  RetrigOPM2 = LL_TIM_OCMODE_RETRIG_OPM2,
  CombinedPWM1 = LL_TIM_OCMODE_COMBINED_PWM1,
  CombinedPWM2 = LL_TIM_OCMODE_COMBINED_PWM2,
  AsymmetricPWM1 = LL_TIM_OCMODE_ASSYMETRIC_PWM1,
  AsymmetricPWM2 = LL_TIM_OCMODE_ASSYMETRIC_PWM2
};

enum class OutputComparePolarity : uint32_t
{
  High = LL_TIM_OCPOLARITY_HIGH,
  Low = LL_TIM_OCPOLARITY_LOW
};

enum class OutputCompareIdleState : uint32_t
{
  Low = LL_TIM_OCIDLESTATE_LOW,
  High = LL_TIM_OCIDLESTATE_HIGH
};
}  // namespace timer

template <timer::TimerAddress TAddress>
class Timer {
 private:
  TIM_TypeDef* TIMAddress() const {
    return reinterpret_cast<TIM_TypeDef*>(TAddress);
  }

 public:
  const void setCounterMode(timer::CounterMode mode) const {
    if (IS_TIM_COUNTER_MODE_SELECT_INSTANCE(TIMAddress())) {
      LL_TIM_SetCounterMode(TIMAddress(), (uint32_t)mode);
    } else {
      assert_param(0);
    }
  }

  const void setClockDivision(timer::ClockDivision clockDivision) const {
    if (IS_TIM_CLOCK_DIVISION_INSTANCE(TIMAddress())) {
      LL_TIM_SetClockDivision(TIMAddress(), (uint32_t)clockDivision);
    } else {
      assert_param(0);
    }
  }

  const void setAutoReload(uint32_t autoReload) const {
    LL_TIM_SetAutoReload(TIMAddress(), autoReload);
  }

  const void setPrescaler(uint32_t prescaler) const {
    LL_TIM_SetPrescaler(TIMAddress(), prescaler);
  }

  const void setPrescaler(uint32_t timerInputClockFrequency, uint32_t counterClockFrequency) const {
    uint32_t prescaler = __LL_TIM_CALC_PSC(timerInputClockFrequency, counterClockFrequency);
    LL_TIM_SetPrescaler(TIMAddress(), prescaler);
  }

  const void setRepetitionCounter(uint32_t repetitionCounter) const {
    if (IS_TIM_REPETITION_COUNTER_INSTANCE(TIMAddress())) {
      LL_TIM_SetRepetitionCounter(TIMAddress(), repetitionCounter);
    } else {
      assert_param(0);
    }
  }

  const void disableAutoReloadPreload() const {
    LL_TIM_DisableARRPreload(TIMAddress());
  }

  const void setTriggerOutput(timer::TriggerOutput triggerOutput) const {
    LL_TIM_SetTriggerOutput(TIMAddress(), (uint32_t)triggerOutput);
  }

  const void disableMasterSlaveMode() const {
    LL_TIM_DisableMasterSlaveMode(TIMAddress());
  }

  const void disableCaptureCompareChannel(timer::ChannelN channel) const {
    LL_TIM_CC_DisableChannel(TIMAddress(), (uint32_t)channel);
  }

  const void setInputCaptureActiveInput(timer::Channel channel, timer::InputCaptureActiveInput activeInput) const {
    LL_TIM_IC_SetActiveInput(TIMAddress(), (uint32_t)channel, (uint32_t)activeInput);
  }

  const void setInputCapturePrescaler(timer::Channel channel, timer::InputCapturePrescaler prescaler) const {
    LL_TIM_IC_SetPrescaler(TIMAddress(), (uint32_t)channel, (uint32_t)prescaler);
  }

  const void setInputCaptureFilter(timer::Channel channel, timer::InputCaptureFilter filter) const {
    LL_TIM_IC_SetFilter(TIMAddress(), (uint32_t)channel, (uint32_t)filter);
  }

  const void setInputCapturePolarity(timer::Channel channel, timer::InputCapturePolarity polarity) const {
    LL_TIM_IC_SetPolarity(TIMAddress(), (uint32_t)channel, (uint32_t)polarity);
  }

  const void enableOutputComparePreload(timer::Channel channel) const {
    LL_TIM_OC_EnablePreload(TIMAddress(), (uint32_t)channel);
  }

  const void setOutputCompareMode(timer::Channel channel, timer::OutputCompareMode mode) const {
    LL_TIM_OC_SetMode(TIMAddress(), (uint32_t)channel, (uint32_t)mode);
  }

  const void setOutputComparePolarity(timer::ChannelN channel, timer::OutputComparePolarity polarity) const {
    LL_TIM_OC_SetPolarity(TIMAddress(), (uint32_t)channel, (uint32_t)polarity);
  }

  const void setOutputCompareIdleState(timer::ChannelN channel, timer::OutputCompareIdleState idleState) const {
    LL_TIM_OC_SetIdleState(TIMAddress(), (uint32_t)channel, (uint32_t)idleState);
  }

  const void setOutputCompareValue(timer::Channel channel, uint32_t value) {
    switch (channel) {
      case timer::Channel::Channel1:
        LL_TIM_OC_SetCompareCH1(TIMAddress(), value);
        break;
      case timer::Channel::Channel2:
        LL_TIM_OC_SetCompareCH2(TIMAddress(), value);
        break;
      case timer::Channel::Channel3:
        LL_TIM_OC_SetCompareCH3(TIMAddress(), value);
        break;
      case timer::Channel::Channel4:
        LL_TIM_OC_SetCompareCH4(TIMAddress(), value);
        break;
      case timer::Channel::Channel5:
        LL_TIM_OC_SetCompareCH5(TIMAddress(), value);
        break;
      default:
        assert_param(0);
        break;
    }
  }

  const void disableOutputCompareFast(timer::Channel channel) {
    LL_TIM_OC_DisableFast(TIMAddress(), (uint32_t)channel);
  }

  const void enableCaptureCompareInterrupt(timer::Channel channel) {
    switch (channel) {
      case timer::Channel::Channel1:
        LL_TIM_EnableIT_CC1(TIMAddress());
        break;
      case timer::Channel::Channel2:
        LL_TIM_EnableIT_CC2(TIMAddress());
        break;
      case timer::Channel::Channel3:
        LL_TIM_EnableIT_CC3(TIMAddress());
        break;
      case timer::Channel::Channel4:
        LL_TIM_EnableIT_CC4(TIMAddress());
        break;
      default:
        assert_param(0);
        break;
    }
  }

  const void enableCaptureCompareDMARequest(timer::Channel channel) {
    switch (channel) {
      case timer::Channel::Channel1:
        LL_TIM_EnableDMAReq_CC1(TIMAddress());
        break;
      case timer::Channel::Channel2:
        LL_TIM_EnableDMAReq_CC2(TIMAddress());
        break;
      case timer::Channel::Channel3:
        LL_TIM_EnableDMAReq_CC3(TIMAddress());
        break;
      case timer::Channel::Channel4:
        LL_TIM_EnableDMAReq_CC4(TIMAddress());
        break;
      default:
        assert_param(0);
        break;
    }
  }

  const void enableChannel(timer::ChannelN channel) {
    LL_TIM_CC_EnableChannel(TIMAddress(), (uint32_t)channel);
  }

  const void enableCounter() {
    LL_TIM_EnableCounter(TIMAddress());
  }

  volatile void* getCaptureCompare1RegisterAddress() const {
    return &(TIMAddress()->CCR1);
  }

  const void enableClock(hal::Clocks& clocks) const {
    switch (TAddress) {
      case timer::TimerAddress::TIM1Address:
        clocks.enableTIM1Clock();
        break;
      case timer::TimerAddress::TIM2Address:
        clocks.enableTIM2Clock();
        break;
      case timer::TimerAddress::TIM3Address:
        clocks.enableTIM3Clock();
        break;
      case timer::TimerAddress::TIM14Address:
        clocks.enableTIM14Clock();
        break;
      case timer::TimerAddress::TIM16Address:
        clocks.enableTIM16Clock();
        break;
      case timer::TimerAddress::TIM17Address:
        clocks.enableTIM17Clock();
        break;
      default:
        assert_param(0);
        break;
    }
  }
};
}  // namespace hal

#endif
