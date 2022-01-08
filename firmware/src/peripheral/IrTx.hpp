#ifndef _PERIPHERAL_IR_TX_HPP_
#define _PERIPHERAL_IR_TX_HPP_

#include "USART.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "utils/Queue.hpp"

namespace peripheral {
class IrTx {
 private:
  const uint32_t carrierPrescaler = 0;
  const uint32_t signalPrescaler = 10;

  hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin;
  hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer;
  hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer;
  Queue<uint32_t, IR_TX_BUFFER_SAMPLES> txBuffer;
  bool sending;

 public:
  IrTx(
      hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin,
      hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer,
      hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer)
      : irOutPin(irOutPin), irTxCarrierTimer(irTxCarrierTimer), irTxSignalTimer(irTxSignalTimer) {
  }

  void initialize(hal::Clocks& clocks, hal::System& system, hal::NVICHal& nvic) {
    this->irOutPin->enableClock(clocks);
    this->irOutPin->setSpeed(hal::gpio::Speed::Low);
    this->irOutPin->setOutputType(hal::gpio::OutputType::PushPull);
    this->irOutPin->setPull(hal::gpio::Pull::None);
    this->irOutPin->setAlternate(hal::gpio::Alternate::Alt0);
    this->irOutPin->setMode(hal::gpio::Mode::Alternate);

    system.setIRModulationEnvelopeSignalSource(hal::system::IRModulationEnvelopeSignalSource::IR_TIM16);
    system.setIRPolarity(hal::system::IRPolarity::Inverted);

    this->irTxSignalTimer->enableClock(clocks);
    this->irTxSignalTimer->setCounterMode(hal::timer::CounterMode::Up);
    this->irTxSignalTimer->setClockDivision(hal::timer::ClockDivision::DIV_1);
    this->irTxSignalTimer->setAutoReload(65535);
    this->irTxSignalTimer->setPrescaler(0);
    this->irTxSignalTimer->setRepetitionCounter(0);
    this->irTxSignalTimer->disableAutoReloadPreload();
    this->irTxSignalTimer->enableOutputComparePreload(hal::timer::Channel::Channel1);
    this->irTxSignalTimer->disableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
    this->irTxSignalTimer->disableCaptureCompareChannel(hal::timer::ChannelN::Channel1N);
    this->irTxSignalTimer->setOutputCompareMode(hal::timer::Channel::Channel1, hal::timer::OutputCompareMode::PWM1);
    this->irTxSignalTimer->setOutputComparePolarity(
        hal::timer::ChannelN::Channel1, hal::timer::OutputComparePolarity::High);
    this->irTxSignalTimer->setOutputComparePolarity(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputComparePolarity::High);
    // TODO set this to high?
    this->irTxSignalTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
    this->irTxSignalTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
    this->irTxSignalTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);

    this->irTxCarrierTimer->enableClock(clocks);
    this->irTxCarrierTimer->setCounterMode(hal::timer::CounterMode::Up);
    this->irTxCarrierTimer->setClockDivision(hal::timer::ClockDivision::DIV_1);
    this->irTxCarrierTimer->setAutoReload(65535);
    this->irTxCarrierTimer->setPrescaler(0);
    this->irTxCarrierTimer->setRepetitionCounter(0);
    this->irTxCarrierTimer->disableAutoReloadPreload();
    this->irTxCarrierTimer->enableOutputComparePreload(hal::timer::Channel::Channel1);
    this->irTxCarrierTimer->disableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
    this->irTxCarrierTimer->disableCaptureCompareChannel(hal::timer::ChannelN::Channel1N);
    this->irTxCarrierTimer->setOutputCompareMode(hal::timer::Channel::Channel1, hal::timer::OutputCompareMode::PWM1);
    this->irTxCarrierTimer->setOutputComparePolarity(
        hal::timer::ChannelN::Channel1, hal::timer::OutputComparePolarity::High);
    this->irTxCarrierTimer->setOutputComparePolarity(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputComparePolarity::High);
    // TODO set this to high?
    this->irTxCarrierTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
    this->irTxCarrierTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
    this->irTxCarrierTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
    this->irTxCarrierTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);

    nvic.setPriority(hal::nvic::IRQnType::TIM16_Irq, 0);
    nvic.enableInterrupt(hal::nvic::IRQnType::TIM16_Irq);

    this->enableGpio(false);

    this->sending = false;

    // Not sure why we need this but the first transmit is always wrong
    this->reset(38000);
    this->write(1000, 1000);
    this->send();
  }

  void reset(uint32_t carrierFrequency) {
    this->stop();

    // init carrier timer
    this->irTxCarrierTimer->setPrescaler(carrierPrescaler);
    uint32_t autoReload = this->irTxCarrierTimer->setAutoReload(SystemCoreClock, carrierPrescaler, carrierFrequency);
    this->irTxCarrierTimer->setOutputCompareValue(hal::timer::Channel::Channel1, autoReload / 4);  // 25% duty cycle
    this->irTxCarrierTimer->enableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
    this->irTxCarrierTimer->enableAllOutputs();

    // init signal timer
    this->irTxSignalTimer->setPrescaler(signalPrescaler);
    this->irTxSignalTimer->setAutoReload(65000);
    this->irTxSignalTimer->disableOutputComparePreload(hal::timer::Channel::Channel1);
    this->irTxSignalTimer->enableUpdateInterrupt();
    this->irTxSignalTimer->enableCaptureCompareChannel(hal::timer::ChannelN::Channel1);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 30000);
    this->irTxSignalTimer->enableAllOutputs();
  }

  void write(uint32_t t_on, uint32_t t_off) {
    this->txBuffer.push(t_on);
    this->txBuffer.push(t_off);
  }

  void send() {
    if (!this->sending) {
      this->sending = true;

      this->nextSignal();
      this->enableGpio(true);
      this->irTxCarrierTimer->enableCounter();
      this->irTxSignalTimer->enableCounter();
    }
  }

  void handleInterrupt() {
    if (this->irTxSignalTimer->isUpdateFlagSet()) {
      this->irTxSignalTimer->clearUpdateFlag();
      this->nextSignal();
    }
  }

  const uint32_t getNumberOfSamplesInBuffer() const {
    return this->txBuffer.getAvailable();
  }

 private:
  void enableGpio(bool enable) const {
    this->irOutPin->setMode(enable ? hal::gpio::Mode::Alternate : hal::gpio::Mode::Output);
    if (enable) {
      this->irOutPin->setOutputPin();
    } else {
      this->irOutPin->resetOutputPin();
    }
  }

  void stop() {
    this->sending = false;
    this->irTxCarrierTimer->disableCounter();
    this->irTxSignalTimer->disableCounter();
    this->enableGpio(false);
  }

  void nextSignal() {
    if (this->txBuffer.getAvailable() < 2) {
      this->stop();
      usartOutput.write("?send complete\n");
      return;
    }

    uint32_t t_on = this->txBuffer.pop();
    uint32_t t_off = this->txBuffer.pop();

    uint32_t on_t = this->irTxSignalTimer->calculateDelay(SystemCoreClock, signalPrescaler, t_on);
    uint32_t off_t = this->irTxSignalTimer->calculateDelay(SystemCoreClock, signalPrescaler, t_off);
    uint32_t total_t = on_t + off_t;

    this->irTxSignalTimer->setAutoReload(total_t);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, on_t);
  }
};
}  // namespace peripheral

#endif
