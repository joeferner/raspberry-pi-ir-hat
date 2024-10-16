#include "CurrentSensor.hpp"

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

void CurrentSensor::initialize(
  hal::Clocks& clocks,
  hal::NVICHal& nvic,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin0>& currentRefPin,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin4>& current0Pin,
  hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin5>& current1Pin) {
  this->adc->enableClock(clocks);

  currentRefPin.enableClock(clocks);
  currentRefPin.setSpeed(hal::gpio::Speed::Low);
  currentRefPin.setPull(hal::gpio::Pull::None);
  currentRefPin.setMode(hal::gpio::Mode::Analog);

  current0Pin.enableClock(clocks);
  current0Pin.setSpeed(hal::gpio::Speed::Low);
  current0Pin.setPull(hal::gpio::Pull::None);
  current0Pin.setMode(hal::gpio::Mode::Analog);

  current1Pin.enableClock(clocks);
  current1Pin.setSpeed(hal::gpio::Speed::Low);
  current1Pin.setPull(hal::gpio::Pull::None);
  current1Pin.setMode(hal::gpio::Mode::Analog);

  this->adc->disable();
  this->adc->setResolution(hal::adc::Resolution::Resolution12Bits);
  this->adc->setDataAlignment(hal::adc::DataAlignment::Right);
  this->adc->setLowPowerMode(hal::adc::LowPowerMode::None);
  this->adc->setClock(hal::adc::Clock::SyncPCLK_DIV2);
  this->adc->setSequencerConfigurable(hal::adc::SequencerConfigurable::Configurable);
  this->adc->pollForReady();
  this->adc->clearChannelConfigurationReadyFlag();
  this->adc->setTriggerSource(hal::adc::TriggerSource::Software);
  this->adc->setSequencerLength(hal::adc::SequencerLength::Enable3Ranks);
  this->adc->setSequencerDiscontinuousMode(hal::adc::SequencerDiscontinuousMode::Rank1);
  this->adc->setContinuousMode(hal::adc::ContinuousMode::Single);
  this->adc->setDMATransfer(hal::adc::DMATransfer::None);
  this->adc->setOverrun(hal::adc::Overrun::Preserved);
  this->adc->setOverSamplingScope(hal::adc::OverSamplingScope::Disable);
  this->adc->setTriggerFrequencyMode(hal::adc::TriggerFrequencyMode::High);
  this->adc->setSamplingTime1(hal::adc::SamplingTime::SamplingTime_12_5);
  this->adc->setSamplingTime2(hal::adc::SamplingTime::SamplingTime_12_5);
  this->adc->disableEndOfUnitaryConversionInterrupt();
  this->adc->disableEndOfSequenceConversionInterrupt();
  this->adc->enableInternalVoltageRegulator();

  this->adc->setSequencerRanks(hal::adc::Rank::Rank1, hal::adc::Channel::Channel0);
  this->adc->setChannelSamplingTime(hal::adc::Channel::Channel0, hal::adc::SamplingTimeCommon::Common1);
  this->adc->setSequencerRanks(hal::adc::Rank::Rank2, hal::adc::Channel::Channel4);
  this->adc->setChannelSamplingTime(hal::adc::Channel::Channel4, hal::adc::SamplingTimeCommon::Common1);
  this->adc->setSequencerRanks(hal::adc::Rank::Rank3, hal::adc::Channel::Channel5);
  this->adc->setChannelSamplingTime(hal::adc::Channel::Channel5, hal::adc::SamplingTimeCommon::Common1);
  this->adc->pollForReady();
  this->adc->clearChannelConfigurationReadyFlag();

  endOfConversion = false;
  endOfSequence = false;
  current_0_mV = current_1_mV = currentReference_mV = 3300 / 2;
  current_0_localMax_mV = current_1_localMax_mV = 0;
  sequenceCount = 0;
  // TODO after PCBs created set this to CurrentReadRank::Reference
  currentReadRank = CurrentReadRank::Current1;
  this->adc->enableEndOfConversionInterrupt();
  this->adc->enableEndOfSequenceInterrupt();
  this->adc->enableOverrunInterrupt();
  this->adc->enable();
  this->adc->startConversion();

  nvic.setPriority(hal::nvic::IRQnType::ADC1_Irq, 0);
  nvic.enableInterrupt(hal::nvic::IRQnType::ADC1_Irq);
}

void CurrentSensor::loop() {
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

void CurrentSensor::handleInterrupt() {
  if (this->adc->isEndOfConversionFlagSet()) {
    this->adc->clearEndOfConversionFlag();
    this->handleEndOfConversion();
  }

  if (this->adc->isEndOfSequenceFlagSet()) {
    this->adc->clearEndOfSequenceFlag();
    this->handleEndOfSequence();
  }

  if (this->adc->isOverrunFlagSet()) {
    this->adc->clearOverrunFlag();
    this->handleOverrunError();
  }
}

const uint16_t CurrentSensor::get(CurrentSensorInput input) const {
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

void CurrentSensor::handleEndOfConversion() {
  this->endOfConversion = true;
}

void CurrentSensor::handleEndOfSequence() {
  this->endOfSequence = true;
}

void CurrentSensor::handleOverrunError() {
  usartOutput.write("-ERR current sensor overrun\n");
}

}  // namespace peripheral
