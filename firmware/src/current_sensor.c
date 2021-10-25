#include "current_sensor.h"

#include <stdbool.h>
#include <stddef.h>

#include "config.h"

#define VREF_mV (3300U)
#define DECAY (0.1f)

static uint16_t* channel;
static uint16_t current_reference_mV;
static uint16_t current_1_mV;
static uint16_t current_2_mV;
static volatile bool end_of_conversion;
static volatile bool end_of_sequence;

void current_sensor_setup() {
  end_of_conversion = false;
  end_of_sequence = false;
  channel = &current_reference_mV;
  LL_ADC_EnableIT_EOC(ADC1);
  LL_ADC_EnableIT_EOS(ADC1);
  LL_ADC_EnableIT_OVR(ADC1);
  LL_ADC_Enable(ADC1);
  LL_ADC_REG_StartConversion(ADC1);
}

void current_sensor_loop() {
  bool start_conversion = false;

  if (end_of_conversion) {
    assert_param(channel != NULL);
    uint16_t raw_data = LL_ADC_REG_ReadConversionData12(ADC1);
    uint16_t data_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(VREF_mV, raw_data, LL_ADC_RESOLUTION_12B);
    assert_param(channel != NULL);
    *channel = (DECAY * ((float)data_mV)) + ((1.0f - DECAY) * ((float)*channel));
    if (channel == &current_reference_mV) {
      channel = &current_1_mV;
    } else if (channel == &current_1_mV) {
      channel = &current_2_mV;
    } else {
      channel = NULL;
    }
    end_of_conversion = false;
    start_conversion = true;
  }

  if (end_of_sequence) {
    channel = &current_reference_mV;
    end_of_sequence = false;
  }

  if (start_conversion) {
    LL_ADC_REG_StartConversion(ADC1);
  }
}

void current_sensor_end_of_conversion() { end_of_conversion = true; }

void current_sensor_end_of_sequence() { end_of_sequence = true; }

void current_sensor_overrun_error() {
  const char* str = "-ERR current sensor overrun\n";
  rpi_send_string(str);
  debug_send_string(str);
}
