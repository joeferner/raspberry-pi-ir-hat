#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "main.h"

namespace hal {
namespace timer {
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
}  // namespace timer

class Timer {
 private:
  TIM_TypeDef* timer;

 public:
  Timer(TIM_TypeDef* timer) : timer(timer) {
  }

  const void setCounterMode(timer::CounterMode mode) const;
  const void setClockDivision(timer::ClockDivision clockDivision) const;
  const void setAutoReload(uint32_t autoReload) const;
  const void setPrescaler(uint32_t prescaler) const;
  const void disableAutoReloadPreload() const;
  const void setTriggerOutput(timer::TriggerOutput triggerOutput) const;
  const void disableMasterSlaveMode() const;
  const void setInputCaptureActiveInput(timer::Channel channel, timer::InputCaptureActiveInput activeInput) const;
  const void setInputCapturePrescaler(timer::Channel channel, timer::InputCapturePrescaler prescaler) const;
  const void setInputCaptureFilter(timer::Channel channel, timer::InputCaptureFilter filter) const;
  const void setInputCapturePolarity(timer::Channel channel, timer::InputCapturePolarity polarity) const;
};
}  // namespace hal

#endif
