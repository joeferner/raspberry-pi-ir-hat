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
  uint16_t current_reference_mV;
  uint16_t current_0_mV;
  uint16_t current_0_local_max_mV;
  uint16_t current_1_mV;
  uint16_t current_1_local_max_mV;
  uint16_t sequence_count;
  CurrentReadRank current_read_rank;
  volatile bool end_of_conversion;
  volatile bool end_of_sequence;

 public:
  void initialize() {
    end_of_conversion = false;
    end_of_sequence = false;
    current_0_mV = current_1_mV = current_reference_mV = 3300 / 2;
    current_0_local_max_mV = current_1_local_max_mV = 0;
    sequence_count = 0;
    // TODO after PCBs created set this to CurrentReadRank::Reference
    current_read_rank = CurrentReadRank::Current1;
    LL_ADC_EnableIT_EOC(ADC1);
    LL_ADC_EnableIT_EOS(ADC1);
    LL_ADC_EnableIT_OVR(ADC1);
    LL_ADC_Enable(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
  }

  void loop() {
    bool start_conversion = false;

    if (end_of_conversion) {
      uint16_t raw_data = LL_ADC_REG_ReadConversionData12(ADC1);
      uint32_t data_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(VREF_mV, raw_data, LL_ADC_RESOLUTION_12B);

      if (current_read_rank == CurrentReadRank::Reference) {
        current_reference_mV = ((VREF_DECAY * data_mV) + ((100 - VREF_DECAY) * current_reference_mV)) / 100;
        current_read_rank = CurrentReadRank::Current0;
      } else if (current_read_rank == CurrentReadRank::Current0 || current_read_rank == CurrentReadRank::Current1) {
        uint16_t diff = abs((int32_t)current_reference_mV - (int32_t)data_mV);

        if (current_read_rank == CurrentReadRank::Current0) {
          current_0_local_max_mV = MAX(diff, current_0_local_max_mV);
          current_read_rank = CurrentReadRank::Unknown;  // TODO CurrentReadRank::Current1
        } else if (current_read_rank == CurrentReadRank::Current1) {
          current_1_local_max_mV = MAX(diff, current_1_local_max_mV);
          current_read_rank = CurrentReadRank::Reference;  // TODO CurrentReadRank::Unknown
        } else {
          assert_param(false);
        }
      }

      end_of_conversion = false;
      start_conversion = true;
    }

    if (end_of_sequence) {
      current_read_rank = CurrentReadRank::Current1;  // TODO CurrentReadRank::Reference
      end_of_sequence = false;
      sequence_count++;
      if (sequence_count > LOCAL_SEQUENCE_COUNT) {
        current_0_mV = ((CURRENT_DECAY * current_0_local_max_mV) + ((100 - CURRENT_DECAY) * current_0_mV)) / 100;
        current_0_local_max_mV = 0;
        current_1_mV = ((CURRENT_DECAY * current_1_local_max_mV) + ((100 - CURRENT_DECAY) * current_1_mV)) / 100;
        current_1_local_max_mV = 0;
        sequence_count = 0;
      }
    }

    if (start_conversion) {
      LL_ADC_REG_StartConversion(ADC1);
    }
  }

  void handleInterrupt() {
    // TODO
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
        return this->current_reference_mV;
      default:
        assert_param(0);
        return 0;
    }
  }

 private:
  void handleEndOfConversion() {
    this->end_of_conversion = true;
  }

  void handleEndOfSequence() {
    this->end_of_sequence = true;
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
