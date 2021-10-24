#include "current_sensor.h"

#include <stddef.h>

#include "config.h"

#define VREF_mV (3300U)
#define DECAY (0.1f)

uint16_t* channel;
uint16_t current_reference_mV;
uint16_t current_1_mV;
uint16_t current_2_mV;

void current_sensor_setup() {
  channel = &current_reference_mV;
  LL_ADC_EnableIT_EOC(ADC1);
  LL_ADC_EnableIT_EOS(ADC1);
  LL_ADC_EnableIT_OVR(ADC1);
  LL_ADC_Enable(ADC1);
  LL_ADC_REG_StartConversion(ADC1);
}

void current_sensor_loop() {}

void current_sensor_end_of_conversion() {
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
}

void current_sensor_end_of_sequence() { channel = &current_reference_mV; }
