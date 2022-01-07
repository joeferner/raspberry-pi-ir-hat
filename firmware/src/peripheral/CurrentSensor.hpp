#ifndef _CURRENT_SENSOR_HPP_
#define _CURRENT_SENSOR_HPP_

#include <stdlib.h>

#include "main.h"

const uint32_t VREF_mV = 3300;
const uint16_t VREF_DECAY = 10;
const uint16_t CURRENT_DECAY = 10;

/**
 * Number of samples to take before adding the max value to current results
 */
const uint16_t LOCAL_SEQUENCE_COUNT = 50;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

static uint16_t to_mA(uint16_t mV) {
  uint32_t data = MAX(0, mV - 15);
  return (data * 890) / 1000;
}

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
  void initialize() {
    // TODO ADC
    //   /* Peripheral clock enable */
    //   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);

    //   LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    //   /**ADC1 GPIO Configuration
    //   PA0   ------> ADC1_IN0
    //   PA4   ------> ADC1_IN4
    //   PA5   ------> ADC1_IN5
    //   */
    //   GPIO_InitStruct.Pin = CURREF_Pin;
    //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    //   LL_GPIO_Init(CURREF_GPIO_Port, &GPIO_InitStruct);

    //   GPIO_InitStruct.Pin = CUR1_Pin;
    //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    //   LL_GPIO_Init(CUR1_GPIO_Port, &GPIO_InitStruct);

    //   GPIO_InitStruct.Pin = CUR2_Pin;
    //   GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    //   GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    //   LL_GPIO_Init(CUR2_GPIO_Port, &GPIO_InitStruct);

    //   /* ADC1 interrupt Init */
    //   NVIC_SetPriority(ADC1_IRQn, 0);
    //   NVIC_EnableIRQ(ADC1_IRQn);

    //   /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    //   */

    // #define ADC_CHANNEL_CONF_RDY_TIMEOUT_MS ( 1U)
    // #if (USE_TIMEOUT == 1)
    //   uint32_t Timeout; /* Variable used for Timeout management */
    // #endif /* USE_TIMEOUT */

    //   ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
    //   ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
    //   ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    //   ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    //   LL_ADC_Init(ADC1, &ADC_InitStruct);
    //   LL_ADC_REG_SetSequencerConfigurable(ADC1, LL_ADC_REG_SEQ_CONFIGURABLE);

    //   /* Poll for ADC channel configuration ready */
    // #if (USE_TIMEOUT == 1)
    //   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
    // #endif /* USE_TIMEOUT */
    //   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
    //   {
    // #if (USE_TIMEOUT == 1)
    //     /* Check Systick counter flag to decrement the time-out value */
    //     if (LL_SYSTICK_IsActiveCounterFlag())
    //     {
    //       if (Timeout-- == 0)
    //       {
    //         Error_Handler();
    //       }
    //     }
    // #endif /* USE_TIMEOUT */
    //   }
    //   /* Clear flag ADC channel configuration ready */
    //   LL_ADC_ClearFlag_CCRDY(ADC1);
    //   ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    //   ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
    //   ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_1RANK;
    //   ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
    //   ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
    //   ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
    //   LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
    //   LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
    //   LL_ADC_SetTriggerFrequencyMode(ADC1, LL_ADC_CLOCK_FREQ_MODE_HIGH);
    //   LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_1, LL_ADC_SAMPLINGTIME_12CYCLES_5);
    //   LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_COMMON_2, LL_ADC_SAMPLINGTIME_12CYCLES_5);
    //   LL_ADC_DisableIT_EOC(ADC1);
    //   LL_ADC_DisableIT_EOS(ADC1);

    //   /* Enable ADC internal voltage regulator */
    //   LL_ADC_EnableInternalRegulator(ADC1);
    //   /* Delay for ADC internal voltage regulator stabilization. */
    //   /* Compute number of CPU cycles to wait for, from delay in us. */
    //   /* Note: Variable divided by 2 to compensate partially */
    //   /* CPU processing cycles (depends on compilation optimization). */
    //   /* Note: If system core clock frequency is below 200kHz, wait time */
    //   /* is only a few CPU processing cycles. */
    //   uint32_t wait_loop_index;
    //   wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    //   while (wait_loop_index != 0)
    //   {
    //     wait_loop_index--;
    //   }
    //   /** Configure Regular Channel
    //   */
    //   LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    //   LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_COMMON_1);
    //   /** Configure Regular Channel
    //   */
    //   LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_4);

    //   /* Poll for ADC channel configuration ready */
    // #if (USE_TIMEOUT == 1)
    //   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
    // #endif /* USE_TIMEOUT */
    //   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
    //   {
    // #if (USE_TIMEOUT == 1)
    //     /* Check Systick counter flag to decrement the time-out value */
    //     if (LL_SYSTICK_IsActiveCounterFlag())
    //     {
    //       if (Timeout-- == 0)
    //       {
    //         Error_Handler();
    //       }
    //     }
    // #endif /* USE_TIMEOUT */
    //   }
    //   /* Clear flag ADC channel configuration ready */
    //   LL_ADC_ClearFlag_CCRDY(ADC1);
    //   LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_COMMON_1);
    //   /** Configure Regular Channel
    //   */
    //   LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_5);

    //   /* Poll for ADC channel configuration ready */
    // #if (USE_TIMEOUT == 1)
    //   Timeout = ADC_CHANNEL_CONF_RDY_TIMEOUT_MS;
    // #endif /* USE_TIMEOUT */
    //   while (LL_ADC_IsActiveFlag_CCRDY(ADC1) == 0)
    //   {
    // #if (USE_TIMEOUT == 1)
    //     /* Check Systick counter flag to decrement the time-out value */
    //     if (LL_SYSTICK_IsActiveCounterFlag())
    //     {
    //       if (Timeout-- == 0)
    //       {
    //         Error_Handler();
    //       }
    //     }
    // #endif /* USE_TIMEOUT */
    //   }
    //   /* Clear flag ADC channel configuration ready */
    //   LL_ADC_ClearFlag_CCRDY(ADC1);
    //   LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_COMMON_1);

    endOfConversion = false;
    endOfSequence = false;
    current_0_mV = current_1_mV = currentReference_mV = 3300 / 2;
    current_0_localMax_mV = current_1_localMax_mV = 0;
    sequenceCount = 0;
    // TODO after PCBs created set this to CurrentReadRank::Reference
    currentReadRank = CurrentReadRank::Current1;
    LL_ADC_EnableIT_EOC(ADC1);
    LL_ADC_EnableIT_EOS(ADC1);
    LL_ADC_EnableIT_OVR(ADC1);
    LL_ADC_Enable(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
  }

  void loop() {
    bool start_conversion = false;

    if (endOfConversion) {
      uint16_t raw_data = LL_ADC_REG_ReadConversionData12(ADC1);
      uint32_t data_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(VREF_mV, raw_data, LL_ADC_RESOLUTION_12B);

      if (currentReadRank == CurrentReadRank::Reference) {
        currentReference_mV = ((VREF_DECAY * data_mV) + ((100 - VREF_DECAY) * currentReference_mV)) / 100;
        currentReadRank = CurrentReadRank::Current0;
      } else if (currentReadRank == CurrentReadRank::Current0 || currentReadRank == CurrentReadRank::Current1) {
        uint16_t diff = abs((int32_t)currentReference_mV - (int32_t)data_mV);

        if (currentReadRank == CurrentReadRank::Current0) {
          current_0_localMax_mV = MAX(diff, current_0_localMax_mV);
          currentReadRank = CurrentReadRank::Unknown;  // TODO CurrentReadRank::Current1
        } else if (currentReadRank == CurrentReadRank::Current1) {
          current_1_localMax_mV = MAX(diff, current_1_localMax_mV);
          currentReadRank = CurrentReadRank::Reference;  // TODO CurrentReadRank::Unknown
        } else {
          assert_param(false);
        }
      }

      endOfConversion = false;
      start_conversion = true;
    }

    if (endOfSequence) {
      currentReadRank = CurrentReadRank::Current1;  // TODO CurrentReadRank::Reference
      endOfSequence = false;
      sequenceCount++;
      if (sequenceCount > LOCAL_SEQUENCE_COUNT) {
        current_0_mV = ((CURRENT_DECAY * current_0_localMax_mV) + ((100 - CURRENT_DECAY) * current_0_mV)) / 100;
        current_0_localMax_mV = 0;
        current_1_mV = ((CURRENT_DECAY * current_1_localMax_mV) + ((100 - CURRENT_DECAY) * current_1_mV)) / 100;
        current_1_localMax_mV = 0;
        sequenceCount = 0;
      }
    }

    if (start_conversion) {
      LL_ADC_REG_StartConversion(ADC1);
    }
  }

  void handleInterrupt() {
    // TODO ADC
    if (LL_ADC_IsActiveFlag_EOC(ADC1)) {
      LL_ADC_ClearFlag_EOC(ADC1);
      this->handleEndOfConversion();
    }

    if (LL_ADC_IsActiveFlag_EOS(ADC1)) {
      LL_ADC_ClearFlag_EOS(ADC1);
      this->handleEndOfSequence();
    }

    if (LL_ADC_IsActiveFlag_OVR(ADC1)) {
      LL_ADC_ClearFlag_OVR(ADC1);
      this->handleOverrunError();
    }
  }

  const uint16_t get(CurrentSensorInput input) const {
    switch (input) {
      case CurrentSensorInput::Current0:
        return to_mA(this->current_0_mV);
      case CurrentSensorInput::Current1:
        return to_mA(this->current_1_mV);
      case CurrentSensorInput::Reference:
        return this->currentReference_mV;
      default:
        assert_param(0);
        return 0;
    }
  }

 private:
  void handleEndOfConversion() {
    this->endOfConversion = true;
  }

  void handleEndOfSequence() {
    this->endOfSequence = true;
  }

  void handleOverrunError() {
    // TODO ADC
    // const char* str = "-ERR current sensor overrun\n";
    // rpi_send_string(str);
    // debug_send_string(str);
  }
};
}  // namespace peripheral

#endif
