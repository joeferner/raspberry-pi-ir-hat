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
    // TODO set this to high?
    this->irTxSignalTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
    this->irTxSignalTimer->setOutputCompareIdleState(
        hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
    this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
    this->irTxSignalTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);

    this->irTxCarrierTimer->enableClock(*this->clocks);
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

void IrTx::reset(uint32_t carrierFrequency) {
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

void IrTx::delayMicros(uint32_t microseconds) {
    this->clocks->delayMicros(microseconds);
}

void IrTx::write(uint32_t t_on, uint32_t t_off) {
    while (this->txBuffer.isFull()) {
        this->send();
    }
    this->txBuffer.push(t_on);
    while (this->txBuffer.isFull()) {
        this->send();
    }
    this->txBuffer.push(t_off);
}

void IrTx::send() {
    if (!this->sending) {
        this->sending = true;

        this->nextSignal();
        this->enableGpio(true);
        this->irTxCarrierTimer->enableCounter();
        this->irTxSignalTimer->enableCounter();
    }
}

void IrTx::sendAndWait() {
    this->send();
    while(this->sending);
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

void IrTx::enableGpio(bool enable) const {
    this->irOutPin->setMode(enable ? hal::gpio::Mode::Alternate : hal::gpio::Mode::Output);
    if (enable) {
        this->irOutPin->setOutputPin();
    } else {
        this->irOutPin->resetOutputPin();
    }
}

void IrTx::stop() {
    this->sending = false;
    this->irTxCarrierTimer->disableCounter();
    this->irTxSignalTimer->disableCounter();
    this->enableGpio(false);
}

void IrTx::nextSignal() {
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

void IrTx::reloadWatchdogCounter() const {
    this->iwdg->reloadCounter();
}

}