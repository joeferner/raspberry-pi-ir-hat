#include "current_sensor.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "main.h"

#define VREF_mV (3300U)
#define VREF_DECAY 10
#define CURRENT_DECAY 10

/**
 * Number of samples to take before adding the max value to current results
 */
#define LOCAL_SEQUENCE_COUNT 50
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

typedef enum
{
  CurrentReadRank_Reference,
  CurrentReadRank_Current1,
  CurrentReadRank_Current0,
  CurrentReadRank_Unknown,
} CurrentReadRank;

static uint16_t current_reference_mV;
static uint16_t current_0_mV;
static uint16_t current_0_local_max_mV;
static uint16_t current_1_mV;
static uint16_t current_1_local_max_mV;
static uint16_t sequence_count;
static CurrentReadRank current_read_rank;
static volatile bool end_of_conversion;
static volatile bool end_of_sequence;

static uint16_t to_mA(uint16_t mV);

void current_sensor_setup() {
  end_of_conversion = false;
  end_of_sequence = false;
  current_0_mV = current_1_mV = current_reference_mV = 3300 / 2;
  current_0_local_max_mV = current_1_local_max_mV = 0;
  sequence_count = 0;
  current_read_rank = CurrentReadRank_Current1;  // CurrentReadRank_Reference
  LL_ADC_EnableIT_EOC(ADC1);
  LL_ADC_EnableIT_EOS(ADC1);
  LL_ADC_EnableIT_OVR(ADC1);
  LL_ADC_Enable(ADC1);
  LL_ADC_REG_StartConversion(ADC1);
}

void current_sensor_loop() {
  bool start_conversion = false;

  if (end_of_conversion) {
    uint16_t raw_data = LL_ADC_REG_ReadConversionData12(ADC1);
    uint32_t data_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(VREF_mV, raw_data, LL_ADC_RESOLUTION_12B);

    if (current_read_rank == CurrentReadRank_Reference) {
      current_reference_mV = ((VREF_DECAY * data_mV) + ((100 - VREF_DECAY) * current_reference_mV)) / 100;
      current_read_rank = CurrentReadRank_Current0;
    } else if (current_read_rank == CurrentReadRank_Current0 || current_read_rank == CurrentReadRank_Current1) {
      uint16_t diff = abs((int32_t)current_reference_mV - (int32_t)data_mV);

      if (current_read_rank == CurrentReadRank_Current0) {
        current_0_local_max_mV = MAX(diff, current_0_local_max_mV);
        current_read_rank = CurrentReadRank_Unknown;  // TODO CurrentReadRank_Current1
      } else if (current_read_rank == CurrentReadRank_Current1) {
        current_1_local_max_mV = MAX(diff, current_1_local_max_mV);
        current_read_rank = CurrentReadRank_Reference;  // TODO CurrentReadRank_Unknown
      } else {
        assert_param(false);
      }
    }

    end_of_conversion = false;
    start_conversion = true;
  }

  if (end_of_sequence) {
    current_read_rank = CurrentReadRank_Current1;  // TODO CurrentReadRank_Reference
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

void current_sensor_end_of_conversion() {
  end_of_conversion = true;
}

void current_sensor_end_of_sequence() {
  end_of_sequence = true;
}

uint16_t current_sensor_get_ref() {
  return current_reference_mV;
}

uint16_t current_sensor_get0() {
  return to_mA(current_0_mV);
}

uint16_t current_sensor_get1() {
  return to_mA(current_1_mV);
}

static uint16_t to_mA(uint16_t mV) {
  uint32_t data = MAX(0, mV - 15);
  return (data * 890) / 1000;
}