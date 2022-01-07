#ifndef _PERIPHERAL_IR_TX_HPP_
#define _PERIPHERAL_IR_TX_HPP_

#include <etl/queue.h>

#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/NVIC.hpp"
#include "hal/Timer.hpp"
#include "hal/System.hpp"
#include "USART.hpp"

extern peripheral::USART<hal::usart::USARTAddress::USART1Address, DEBUG_TX_BUFFER_SIZE, DEBUG_RX_BUFFER_SIZE> debugUsart;

namespace peripheral {
class IrTx {
private:
    hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin;
    hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer;
    hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer;
    etl::queue<uint32_t, IR_TX_BUFFER_SAMPLES> txBuffer;
    bool sending;

public:
    IrTx(
        hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin,
        hal::Timer<hal::timer::TimerAddress::TIM17Address>* irTxCarrierTimer,
        hal::Timer<hal::timer::TimerAddress::TIM16Address>* irTxSignalTimer
    ) :irOutPin(irOutPin), irTxCarrierTimer(irTxCarrierTimer), irTxSignalTimer(irTxSignalTimer) {
    }

    const void initialize(
        hal::Clocks& clocks,
        hal::System& system,
        hal::NVICHal& nvic
    ) {
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
        this->irTxSignalTimer->setOutputComparePolarity(hal::timer::ChannelN::Channel1, hal::timer::OutputComparePolarity::High);
        this->irTxSignalTimer->setOutputComparePolarity(hal::timer::ChannelN::Channel1N, hal::timer::OutputComparePolarity::High);
        // TODO set this to high?
        this->irTxSignalTimer->setOutputCompareIdleState(hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
        this->irTxSignalTimer->setOutputCompareIdleState(hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
        this->irTxSignalTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
        this->irTxSignalTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);
        // TODO
        // TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
        // TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
        // TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
        // TIM_BDTRInitStruct.DeadTime = 0;
        // TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
        // TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
        // TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
        // TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
        // LL_TIM_BDTR_Init(TIM16, &TIM_BDTRInitStruct);

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
        this->irTxCarrierTimer->setOutputComparePolarity(hal::timer::ChannelN::Channel1, hal::timer::OutputComparePolarity::High);
        this->irTxCarrierTimer->setOutputComparePolarity(hal::timer::ChannelN::Channel1N, hal::timer::OutputComparePolarity::High);
        // TODO set this to high?
        this->irTxCarrierTimer->setOutputCompareIdleState(hal::timer::ChannelN::Channel1, hal::timer::OutputCompareIdleState::Low);
        this->irTxCarrierTimer->setOutputCompareIdleState(hal::timer::ChannelN::Channel1N, hal::timer::OutputCompareIdleState::Low);
        this->irTxCarrierTimer->setOutputCompareValue(hal::timer::Channel::Channel1, 0);
        this->irTxCarrierTimer->disableOutputCompareFast(hal::timer::Channel::Channel1);
        // TODO
        // TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
        // TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
        // TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
        // TIM_BDTRInitStruct.DeadTime = 0;
        // TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
        // TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
        // TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
        // TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
        // LL_TIM_BDTR_Init(TIM17, &TIM_BDTRInitStruct);

        nvic.setPriority(hal::nvic::IRQnType::TIM16_Irq, 0);
        nvic.enableInterrupt(hal::nvic::IRQnType::TIM16_Irq);

        this->enableGpio(false);

        this->sending = false;

        // Not sure why we need this but the first transmit is always wrong
        this->reset(38000);
        this->write(1000, 1000);
        this->send();
    }

    const void reset(uint32_t carrierFrequency) {
        this->stop();

        // init carrier timer
        uint32_t autoReload = __LL_TIM_CALC_ARR(SystemCoreClock, IR_OUT_CARRIER_PRESCALER, carrierFrequency);
        LL_TIM_SetPrescaler(IR_OUT_CARRIER_TIMER, IR_OUT_CARRIER_PRESCALER);
        LL_TIM_SetAutoReload(IR_OUT_CARRIER_TIMER, autoReload);
        LL_TIM_CC_EnableChannel(IR_OUT_CARRIER_TIMER, IR_OUT_CARRIER_CHANNEL);
        IR_OUT_CARRIER_TIM_OC_SetCompare(autoReload / 4);  // 25% duty cycle
        LL_TIM_EnableAllOutputs(IR_OUT_CARRIER_TIMER);

        // init signal timer
        LL_TIM_SetPrescaler(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_PRESCALER);
        LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, 65000);
        LL_TIM_OC_DisablePreload(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_CHANNEL);
        LL_TIM_EnableIT_UPDATE(IR_OUT_SIGNAL_TIMER);
        LL_TIM_CC_EnableChannel(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_CHANNEL);
        IR_OUT_SIGNAL_TIM_OC_SetCompare(30000);
        LL_TIM_EnableAllOutputs(IR_OUT_SIGNAL_TIMER);
    }

    const void write(uint32_t t_on, uint32_t t_off) {
        this->txBuffer.push(t_on);
        this->txBuffer.push(t_off);
    }

    const void send() {
        if (!this->sending) {
            this->sending = true;

            this->nextSignal();
            this->enableGpio(true);
            LL_TIM_EnableCounter(IR_OUT_CARRIER_TIMER);
            LL_TIM_EnableCounter(IR_OUT_SIGNAL_TIMER);
        }
    }

    const void handleInterrupt() {
        if (LL_TIM_IsActiveFlag_UPDATE(IR_OUT_SIGNAL_TIMER)) {
            LL_TIM_ClearFlag_UPDATE(IR_OUT_SIGNAL_TIMER);
            this->nextSignal();
        }
    }

private:
    const void enableGpio(bool enable)const {
        this->irOutPin->setMode(enable ? hal::gpio::Mode::Alternate : hal::gpio::Mode::Output);
        if (enable) {
            this->irOutPin->setOutputPin();
        } else {
            this->irOutPin->resetOutputPin();
        }
    }

    const void stop() {
        this->sending = false;
        LL_TIM_DisableCounter(IR_OUT_CARRIER_TIMER);
        LL_TIM_DisableCounter(IR_OUT_SIGNAL_TIMER);
        this->enableGpio(false);
    }

    const void nextSignal() {
        if (this->txBuffer.available() < 2) {
            this->stop();
            debugUsart.write("?send complete\n");
            return;
        }

        uint32_t t_on = this->txBuffer.front();
        this->txBuffer.pop();
        uint32_t t_off = this->txBuffer.front();
        this->txBuffer.pop();

        uint32_t on_t = __LL_TIM_CALC_DELAY(SystemCoreClock, IR_OUT_SIGNAL_PRESCALER, t_on);
        uint32_t off_t = __LL_TIM_CALC_DELAY(SystemCoreClock, IR_OUT_SIGNAL_PRESCALER, t_off);
        uint32_t total_t = on_t + off_t;

        LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, total_t);
        IR_OUT_SIGNAL_TIM_OC_SetCompare(on_t);
    }
};
}

#endif
