#ifndef _ADC_HPP_
#define _ADC_HPP_

#include "hal/Clocks.hpp"
#include "main.h"

namespace hal {
namespace adc {
enum class ADCAddress : uint32_t
{ ADC1Address = ADC1_BASE };

enum class Resolution : uint32_t
{
  Resolution12Bits = LL_ADC_RESOLUTION_12B,
  Resolution10Bits = LL_ADC_RESOLUTION_10B,
  Resolution8Bits = LL_ADC_RESOLUTION_8B,
  Resolution6Bits = LL_ADC_RESOLUTION_6B
};

enum class DataAlignment : uint32_t
{
  Right = LL_ADC_DATA_ALIGN_RIGHT,
  Left = LL_ADC_DATA_ALIGN_LEFT
};

enum class LowPowerMode : uint32_t
{
  None = LL_ADC_LP_MODE_NONE,
  AutoWait = LL_ADC_LP_AUTOWAIT,
  AutoPowerOff = LL_ADC_LP_AUTOPOWEROFF,
  AutoWaitAutoPowerOff = LL_ADC_LP_AUTOWAIT_AUTOPOWEROFF
};

enum class Clock : uint32_t
{
  SyncPCLK_DIV4 = LL_ADC_CLOCK_SYNC_PCLK_DIV4,
  SyncPCLK_DIV2 = LL_ADC_CLOCK_SYNC_PCLK_DIV2,
  SyncPCLK_DIV1 = LL_ADC_CLOCK_SYNC_PCLK_DIV1,
  Async = LL_ADC_CLOCK_ASYNC
};

enum class SequencerConfigurable : uint32_t
{
  Fixed = LL_ADC_REG_SEQ_FIXED,
  Configurable = LL_ADC_REG_SEQ_CONFIGURABLE
};

enum class TriggerSource : uint32_t
{
  Software = LL_ADC_REG_TRIG_SOFTWARE,
  External_TIM1_TRG02 = LL_ADC_REG_TRIG_EXT_TIM1_TRGO2,
  External_TIM1_CH4 = LL_ADC_REG_TRIG_EXT_TIM1_CH4,
  External_TIM2_TRGO = LL_ADC_REG_TRIG_EXT_TIM2_TRGO,
  External_TIM3_TRGO = LL_ADC_REG_TRIG_EXT_TIM3_TRGO,
#ifdef LL_ADC_REG_TRIG_EXT_TIM6_TRGO
  External_TIM6_TRGO = LL_ADC_REG_TRIG_EXT_TIM6_TRGO,
#endif
#ifdef LL_ADC_REG_TRIG_EXT_TIM15_TRGO
  External_TIM15_TRGO = LL_ADC_REG_TRIG_EXT_TIM15_TRGO,
#endif
  External_EXTI_LINE11 = LL_ADC_REG_TRIG_EXT_EXTI_LINE11
};

enum class SequencerLength : uint32_t
{
  Disable = LL_ADC_REG_SEQ_SCAN_DISABLE,
  Enable2Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,
  Enable3Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS,
  Enable4Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,
  Enable5Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS,
  Enable6Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS,
  Enable7Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS,
  Enable8Ranks = LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS
};

enum class SequencerDiscontinuousMode : uint32_t
{
  Disable = LL_ADC_REG_SEQ_DISCONT_DISABLE,
  Rank1 = LL_ADC_REG_SEQ_DISCONT_1RANK
};

enum class ContinuousMode : uint32_t
{
  Single = LL_ADC_REG_CONV_SINGLE,
  Continuous = LL_ADC_REG_CONV_CONTINUOUS
};

enum class DMATransfer : uint32_t
{
  None = LL_ADC_REG_DMA_TRANSFER_NONE,
  Limited = LL_ADC_REG_DMA_TRANSFER_LIMITED,
  Unlimited = LL_ADC_REG_DMA_TRANSFER_UNLIMITED
};

enum class Overrun : uint32_t
{
  Preserved = LL_ADC_REG_OVR_DATA_PRESERVED,
  Overwritten = LL_ADC_REG_OVR_DATA_OVERWRITTEN
};

enum class OverSamplingScope : uint32_t
{
  Disable = LL_ADC_OVS_DISABLE,
  GroupRegularContinued = LL_ADC_OVS_GRP_REGULAR_CONTINUED
};

enum class TriggerFrequencyMode : uint32_t
{
  High = LL_ADC_TRIGGER_FREQ_HIGH,
  Low = LL_ADC_TRIGGER_FREQ_LOW
};

enum class SamplingTime : uint32_t
{
  SamplingTime_1_5 = LL_ADC_SAMPLINGTIME_1CYCLE_5,
  SamplingTime_3_5 = LL_ADC_SAMPLINGTIME_3CYCLES_5,
  SamplingTime_7_5 = LL_ADC_SAMPLINGTIME_7CYCLES_5,
  SamplingTime_12_5 = LL_ADC_SAMPLINGTIME_12CYCLES_5,
  SamplingTime_19_5 = LL_ADC_SAMPLINGTIME_19CYCLES_5,
  SamplingTime_39_5 = LL_ADC_SAMPLINGTIME_39CYCLES_5,
  SamplingTime_79_5 = LL_ADC_SAMPLINGTIME_79CYCLES_5,
  SamplingTime_160_5 = LL_ADC_SAMPLINGTIME_160CYCLES_5
};

enum class Rank : uint32_t
{
  Rank1 = LL_ADC_REG_RANK_1,
  Rank2 = LL_ADC_REG_RANK_2,
  Rank3 = LL_ADC_REG_RANK_3,
  Rank4 = LL_ADC_REG_RANK_4,
  Rank5 = LL_ADC_REG_RANK_5,
  Rank6 = LL_ADC_REG_RANK_6,
  Rank7 = LL_ADC_REG_RANK_7,
  Rank8 = LL_ADC_REG_RANK_8
};

enum class Channel : uint32_t
{
  Channel0 = LL_ADC_CHANNEL_0,
  Channel1 = LL_ADC_CHANNEL_1,
  Channel2 = LL_ADC_CHANNEL_2,
  Channel3 = LL_ADC_CHANNEL_3,
  Channel4 = LL_ADC_CHANNEL_4,
  Channel5 = LL_ADC_CHANNEL_5,
  Channel6 = LL_ADC_CHANNEL_6,
  Channel7 = LL_ADC_CHANNEL_7,
  Channel8 = LL_ADC_CHANNEL_8,
  Channel9 = LL_ADC_CHANNEL_9,
  Channel10 = LL_ADC_CHANNEL_10,
  Channel11 = LL_ADC_CHANNEL_11,
  Channel12 = LL_ADC_CHANNEL_12,
  Channel13 = LL_ADC_CHANNEL_13,
  Channel14 = LL_ADC_CHANNEL_14,
  Channel15 = LL_ADC_CHANNEL_15,
  Channel16 = LL_ADC_CHANNEL_16,
  Channel17 = LL_ADC_CHANNEL_17,
  Channel18 = LL_ADC_CHANNEL_18,
  ChannelVRefInternal = LL_ADC_CHANNEL_VREFINT,
  ChannelTempSensor = LL_ADC_CHANNEL_TEMPSENSOR,
  ChannelVBat = LL_ADC_CHANNEL_VBAT
};

enum class SamplingTimeCommon
{
  Common1 = LL_ADC_SAMPLINGTIME_COMMON_1,
  Common2 = LL_ADC_SAMPLINGTIME_COMMON_2
};
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

  bool isEndOfConversionFlagSet() const {
    return LL_ADC_IsActiveFlag_EOC(ADCAddress());
  }

  void clearEndOfConversionFlag() {
    LL_ADC_ClearFlag_EOC(ADCAddress());
  }

  void enableEndOfSequenceInterrupt() {
    LL_ADC_EnableIT_EOS(ADCAddress());
  }

  bool isEndOfSequenceFlagSet() const {
    return LL_ADC_IsActiveFlag_EOS(ADCAddress());
  }

  void clearEndOfSequenceFlag() {
    LL_ADC_ClearFlag_EOS(ADCAddress());
  }

  void enableOverrunInterrupt() {
    LL_ADC_EnableIT_OVR(ADCAddress());
  }

  bool isOverrunFlagSet() const {
    return LL_ADC_IsActiveFlag_OVR(ADCAddress());
  }

  void clearOverrunFlag() {
    LL_ADC_ClearFlag_OVR(ADCAddress());
  }

  void enable() {
    LL_ADC_Enable(ADCAddress());
  }

  void disable() {
    LL_ADC_Disable(ADCAddress());
  }

  void startConversion() {
    LL_ADC_REG_StartConversion(ADCAddress());
  }

  void setResolution(adc::Resolution resolution) {
    LL_ADC_SetResolution(ADCAddress(), (uint32_t)resolution);
  }

  void setDataAlignment(adc::DataAlignment dataAlignment) {
    LL_ADC_SetDataAlignment(ADCAddress(), (uint32_t)dataAlignment);
  }

  void setLowPowerMode(adc::LowPowerMode lowPowerMode) {
    LL_ADC_SetLowPowerMode(ADCAddress(), (uint32_t)lowPowerMode);
  }

  void setClock(adc::Clock clock) {
    LL_ADC_SetClock(ADCAddress(), (uint32_t)clock);
  }

  void setSequencerConfigurable(adc::SequencerConfigurable sequencerConfigurable) {
    LL_ADC_REG_SetSequencerConfigurable(ADCAddress(), (uint32_t)sequencerConfigurable);
  }

  adc::SequencerConfigurable getSequencerConfigurable() const {
    return (adc::SequencerConfigurable)LL_ADC_REG_GetSequencerConfigurable(ADCAddress());
  }

  void pollForReady() const {
    while (!this->isChannelConfigurationReadyFlagSet()) {
    }
  }

  bool isChannelConfigurationReadyFlagSet() const {
    return LL_ADC_IsActiveFlag_CCRDY(ADCAddress());
  }

  void clearChannelConfigurationReadyFlag() {
    LL_ADC_ClearFlag_CCRDY(ADCAddress());
  }

  void setTriggerSource(adc::TriggerSource triggerSource) {
    LL_ADC_REG_SetTriggerSource(ADCAddress(), (uint32_t)triggerSource);
  }

  void setSequencerLength(adc::SequencerLength sequencerLength) {
    LL_ADC_REG_SetSequencerLength(ADCAddress(), (uint32_t)sequencerLength);
  }

  adc::SequencerLength getSequencerLength() const {
    return (adc::SequencerLength)LL_ADC_REG_GetSequencerLength(ADCAddress());
  }

  void setSequencerDiscontinuousMode(adc::SequencerDiscontinuousMode sequencerDiscontinuousMode) {
    if ((getSequencerConfigurable() == adc::SequencerConfigurable::Fixed) ||
        (getSequencerLength() != adc::SequencerLength::Disable)) {
      LL_ADC_REG_SetSequencerDiscont(ADCAddress(), (uint32_t)sequencerDiscontinuousMode);
    } else {
      assert_param(sequencerDiscontinuousMode == adc::SequencerDiscontinuousMode::Disable);
      LL_ADC_REG_SetSequencerDiscont(ADCAddress(), LL_ADC_REG_SEQ_DISCONT_DISABLE);
    }
  }

  void setContinuousMode(adc::ContinuousMode continuousMode) {
    LL_ADC_REG_SetContinuousMode(ADCAddress(), (uint32_t)continuousMode);
  }

  void setDMATransfer(adc::DMATransfer dmaTransfer) {
    LL_ADC_REG_SetDMATransfer(ADCAddress(), (uint32_t)dmaTransfer);
  }

  void setOverrun(adc::Overrun overrun) {
    LL_ADC_REG_SetOverrun(ADCAddress(), (uint32_t)overrun);
  }

  void setOverSamplingScope(adc::OverSamplingScope overSamplingScope) {
    LL_ADC_SetOverSamplingScope(ADCAddress(), (uint32_t)overSamplingScope);
  }

  void setTriggerFrequencyMode(adc::TriggerFrequencyMode triggerFrequencyMode) {
    LL_ADC_SetTriggerFrequencyMode(ADCAddress(), (uint32_t)triggerFrequencyMode);
  }

  void setSamplingTime1(adc::SamplingTime samplingTime) {
    LL_ADC_SetSamplingTimeCommonChannels(ADCAddress(), LL_ADC_SAMPLINGTIME_COMMON_1, (uint32_t)samplingTime);
  }

  void setSamplingTime2(adc::SamplingTime samplingTime) {
    LL_ADC_SetSamplingTimeCommonChannels(ADCAddress(), LL_ADC_SAMPLINGTIME_COMMON_2, (uint32_t)samplingTime);
  }

  void disableEndOfUnitaryConversionInterrupt() {
    LL_ADC_DisableIT_EOC(ADCAddress());
  }

  void disableEndOfSequenceConversionInterrupt() {
    LL_ADC_DisableIT_EOS(ADCAddress());
  }

  void enableInternalVoltageRegulator() {
    LL_ADC_EnableInternalRegulator(ADCAddress());

    // Delay for ADC internal voltage regulator stabilization.
    // Compute number of CPU cycles to wait for, from delay in us.
    // Note: Variable divided by 2 to compensate partially
    // CPU processing cycles (depends on compilation optimization).
    // Note: If system core clock frequency is below 200kHz, wait time
    // is only a few CPU processing cycles.
    uint32_t wait_loop_index;
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0) {
      wait_loop_index--;
    }
  }

  void setSequencerRanks(adc::Rank rank, adc::Channel channel) {
    LL_ADC_REG_SetSequencerRanks(ADCAddress(), (uint32_t)rank, (uint32_t)channel);
  }

  void setChannelSamplingTime(adc::Channel channel, adc::SamplingTimeCommon common) {
    LL_ADC_SetChannelSamplingTime(ADCAddress(), (uint32_t)channel, (uint32_t)common);
  }

 private:
  ADC_TypeDef* ADCAddress() const {
    return reinterpret_cast<ADC_TypeDef*>(TAddress);
  }
};
}  // namespace hal

#endif
