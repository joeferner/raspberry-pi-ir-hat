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
    etl::queue<uint32_t, IR_TX_BUFFER_SAMPLES> txBuffer;
    bool sending;

public:
    IrTx(
        hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9>* irOutPin
    ) :irOutPin(irOutPin) {
    }

    const void initialize(
        hal::Clocks& clocks,
        hal::System& system
    ) {
        this->irOutPin->enableClock(clocks);
        this->irOutPin->setSpeed(hal::gpio::Speed::Low);
        this->irOutPin->setOutputType(hal::gpio::OutputType::PushPull);
        this->irOutPin->setPull(hal::gpio::Pull::None);
        this->irOutPin->setAlternate(hal::gpio::Alternate::Alt0);
        this->irOutPin->setMode(hal::gpio::Mode::Alternate);

        this->enableGpio(false);
        system.setIRPolarity(hal::system::IRPolarity::Inverted);

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
