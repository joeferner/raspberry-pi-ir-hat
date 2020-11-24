#include <stdbool.h>
#include "config.h"
#include "ir_tx.h"

static uint32_t *_ir_tx_signal = NULL;
static uint32_t _ir_tx_signal_length = 0;
static uint32_t _ir_tx_signal_idx = 0;

static void _ir_tx_next_signal();

static void _ir_tx_enable_gpio(bool enable);

void ir_tx_setup() {
    _ir_tx_enable_gpio(false);
    NVIC_EnableIRQ(IR_OUT_SIGNAL_IRQ);
    NVIC_SetPriority(IR_OUT_SIGNAL_IRQ, 0);
}

/**
 * @brief the caller owns the signal buffer and must retain it until the send is complete which
 *        happens async
 */
void ir_tx_send(uint32_t carrier_freq, uint32_t *signal, size_t signal_length) {
    _ir_tx_signal = signal;
    _ir_tx_signal_length = signal_length;

    // init carrier timer
    uint32_t prescaler = 0; // __LL_TIM_CALC_PSC(SystemCoreClock, carrier_freq);
    uint32_t autoReload = __LL_TIM_CALC_ARR(SystemCoreClock, prescaler, carrier_freq / 10);
    LL_TIM_SetPrescaler(IR_OUT_CARRIER_TIMER, prescaler);
    LL_TIM_SetAutoReload(IR_OUT_CARRIER_TIMER, autoReload);
    LL_TIM_SetCounterMode(IR_OUT_CARRIER_TIMER, LL_TIM_COUNTERMODE_DOWN);
    LL_TIM_CC_EnableChannel(IR_OUT_CARRIER_TIMER, IR_OUT_CARRIER_CHANNEL);
    IR_OUT_CARRIER_TIM_OC_SetCompareCH1(autoReload / 4); // 25% duty cycle
    LL_TIM_ConfigBRK(IR_OUT_CARRIER_TIMER, LL_TIM_BREAK_POLARITY_LOW, LL_TIM_BREAK_FILTER_FDIV1, LL_TIM_BREAK_AFMODE_INPUT);
    LL_TIM_EnableAllOutputs(IR_OUT_CARRIER_TIMER);

    // init signal timer
    LL_TIM_SetCounterMode(IR_OUT_SIGNAL_TIMER, LL_TIM_COUNTERMODE_DOWN);
    LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, 0);
    LL_TIM_OC_DisablePreload(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_CHANNEL);
    LL_TIM_EnableAllOutputs(IR_OUT_SIGNAL_TIMER);
    LL_TIM_EnableIT_UPDATE(IR_OUT_SIGNAL_TIMER);
    LL_TIM_CC_EnableChannel(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_CHANNEL);

    // begin send
    _ir_tx_enable_gpio(true);
    _ir_tx_signal_idx = 0;
    _ir_tx_next_signal();
    LL_TIM_EnableCounter(IR_OUT_CARRIER_TIMER);
    LL_TIM_EnableCounter(IR_OUT_SIGNAL_TIMER);
}

void _ir_tx_next_signal() {
    if (_ir_tx_signal_idx >= _ir_tx_signal_length) {
        _ir_tx_signal_idx = 0;
        LL_TIM_DisableCounter(IR_OUT_CARRIER_TIMER);
        LL_TIM_DisableCounter(IR_OUT_SIGNAL_TIMER);
        _ir_tx_enable_gpio(false);
        return;
    }

    uint32_t on_t = _ir_tx_signal[_ir_tx_signal_idx] * (SystemCoreClock / 100000);
    uint32_t off_t = _ir_tx_signal[_ir_tx_signal_idx + 1] * (SystemCoreClock / 100000);
    uint32_t total_t = on_t + off_t;

    LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, total_t);
    IR_OUT_LL_TIM_OC_SetCompare(on_t);
    _ir_tx_signal_idx += 2;
}

void ir_tx_irq() {
    if (LL_TIM_IsActiveFlag_UPDATE(IR_OUT_SIGNAL_TIMER)) {
        LL_TIM_ClearFlag_UPDATE(IR_OUT_SIGNAL_TIMER);
        _ir_tx_next_signal();
    }
}

static void _ir_tx_enable_gpio(bool enable) {
    LL_GPIO_SetPinMode(IR_OUT_PORT, IR_OUT_PIN, enable ? LL_GPIO_MODE_ALTERNATE : LL_GPIO_MODE_OUTPUT);
    if (enable) {
        LL_GPIO_SetOutputPin(IR_OUT_PORT, IR_OUT_PIN);
    } else {
        LL_GPIO_ResetOutputPin(IR_OUT_PORT, IR_OUT_PIN);
    }
}