#include "IrTx.hpp"
#include "BothUSARTWriter.hpp"

namespace peripheral {

void IrTx::initialize(hal::System& system, hal::NVICHal& nvic) {
    this->irOutPin->enableClock(*this->clocks);
    this->irOutPin->setSpeed(hal::gpio::Speed::Low);
    this->irOutPin->setOutputType(hal::gpio::OutputType::PushPull);
    this->irOutPin->setPull(hal::gpio::Pull::None);
    this->irOutPin->setAlternate(hal::gpio::Alternate::Alt0);
    this->irOutPin->setMode(hal::gpio::Mode::Alternate);

    system.setIRModulationEnvelopeSignalSource(hal::system::IRModulationEnvelopeSignalSource::IR_TIM16);
    system.setIRPolarity(hal::system::IRPolarity::Inverted);

    this->irTxSignalTimer->enableClock(*this->clocks);
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
    this->irTxSignalTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::High);
    this->irTxSignalTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::High);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
    this->irTxSignalTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);

    this->irTxCarrierTimer->enableClock(*this->clocks);
    this->irTxCarrierTimer->setCounterMode(hal::timer::CounterMode::Up);
    this->irTxCarrierTimer->setClockDivision(hal::timer::ClockDivision::DIV_1);
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
    this->irTxCarrierTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::High);
    this->irTxCarrierTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::High);
    this->irTxCarrierTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);

    nvic.setPriority(hal::nvic::IRQnType::TIM16_Irq, 0);
    nvic.enableInterrupt(hal::nvic::IRQnType::TIM16_Irq);

    this->reset(38000);
}

void IrTx::reset(uint32_t carrierFrequency) {
    this->stop();

    // init carrier timer
    this->irTxCarrierTimer->setPrescaler(carrierPrescaler);
    uint32_t autoReload = this->irTxCarrierTimer->setAutoReload(SystemCoreClock, carrierPrescaler, carrierFrequency);
    this->irTxCarrierTimer->setOutputCompareValue(hal::timer::Channel::Channel1, autoReload / 4);  // 25% duty cycle
    this->irTxCarrierTimer->enableCaptureCompareChannel(hal::timer::ChannelN::Channel1);

    // init signal timer
    this->irTxSignalTimer->setPrescaler(signalPrescaler);
    this->irTxSignalTimer->disableOutputComparePreload(hal::timer::Channel::Channel1);
    this->irTxSignalTimer->enableUpdateInterrupt();
    this->irTxSignalTimer->enableCaptureCompareChannel(hal::timer::ChannelN::Channel1);

    this->stop();

    this->irTxCarrierTimer->enableAllOutputs();
    this->irTxSignalTimer->enableAllOutputs();

    this->irTxCarrierTimer->enableCounter();
    this->irTxSignalTimer->enableCounter();
}

void IrTx::delayMicros(uint32_t microseconds) {
    this->clocks->delayMicros(microseconds);
}

void IrTx::write(uint32_t t_on, uint32_t t_off) {
    uint32_t t_on_scaled = this->irTxSignalTimer->calculateDelay(SystemCoreClock, signalPrescaler, t_on);
    uint32_t t_off_scaled = this->irTxSignalTimer->calculateDelay(SystemCoreClock, signalPrescaler, t_off);

    while (this->txBuffer.isFull()) {
    }
    this->txBuffer.push(t_on_scaled);
    while (this->txBuffer.isFull()) {
    }
    this->txBuffer.push(t_off_scaled);
}

void IrTx::waitForSendToComplete() {
    while (!this->txBuffer.isEmpty());
}

void IrTx::handleInterrupt() {
    if (this->irTxSignalTimer->isUpdateFlagSet()) {
        this->irTxSignalTimer->clearUpdateFlag();
        this->nextSignal();
    }
}

uint32_t IrTx::getNumberOfSamplesInBuffer() const {
    return this->txBuffer.getAvailable();
}

void IrTx::stop() {
    this->irTxSignalTimer->setAutoReload(1000);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
}

void IrTx::nextSignal() {
    if (this->txBuffer.getAvailable() < 2) {
        this->stop();
        return;
    }

    uint32_t t_on = this->txBuffer.pop();
    uint32_t t_off = this->txBuffer.pop();
    uint32_t total_t = t_on + t_off;
    this->irTxSignalTimer->setAutoReload(total_t);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, t_on);
}

void IrTx::reloadWatchdogCounter() const {
    this->iwdg->reloadCounter();
}

}