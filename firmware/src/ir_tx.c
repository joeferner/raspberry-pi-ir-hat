#include <stdbool.h>
#include "config.h"
#include "ir_tx.h"

static uint32_t *ir_tx_signal = NULL;
static uint32_t ir_tx_signal_length = 0;
static uint32_t ir_tx_signal_idx = 0;
static uint32_t ir_tx_temp_signal[] = {1000, 1000};

static void ir_tx_next_signal();

static void ir_tx_enable_gpio(bool enable);

void ir_tx_setup() {
    ir_tx_enable_gpio(false);
    LL_SYSCFG_SetIRPolarity(LL_SYSCFG_IR_POL_INVERTED);

    // Not sure why we need this but the first transmit is always wrong
    ir_tx_send(38000, ir_tx_temp_signal, 2);
}

/**
 * @brief the caller owns the signal buffer and must retain it until the send is complete which
 *        happens async
 */
void ir_tx_send(uint32_t carrier_freq, uint32_t *signal, size_t signal_length) {
    ir_tx_signal = signal;
    ir_tx_signal_length = signal_length;

    // init carrier timer
    uint32_t autoReload = __LL_TIM_CALC_ARR(SystemCoreClock, IR_OUT_CARRIER_PRESCALER, carrier_freq);
    LL_TIM_SetPrescaler(IR_OUT_CARRIER_TIMER, IR_OUT_CARRIER_PRESCALER);
    LL_TIM_SetAutoReload(IR_OUT_CARRIER_TIMER, autoReload);
    LL_TIM_CC_EnableChannel(IR_OUT_CARRIER_TIMER, IR_OUT_CARRIER_CHANNEL);
    IR_OUT_CARRIER_TIM_OC_SetCompare(autoReload / 4); // 25% duty cycle
    LL_TIM_EnableAllOutputs(IR_OUT_CARRIER_TIMER);

    // init signal timer
    LL_TIM_SetPrescaler(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_PRESCALER);
    LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, 65000);
    LL_TIM_OC_DisablePreload(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_CHANNEL);
    LL_TIM_EnableIT_UPDATE(IR_OUT_SIGNAL_TIMER);
    LL_TIM_CC_EnableChannel(IR_OUT_SIGNAL_TIMER, IR_OUT_SIGNAL_CHANNEL);
    IR_OUT_SIGNAL_TIM_OC_SetCompare(30000);
    LL_TIM_EnableAllOutputs(IR_OUT_SIGNAL_TIMER);

    // begin send
    ir_tx_signal_idx = 0;
    ir_tx_next_signal();
    ir_tx_enable_gpio(true);
    LL_TIM_EnableCounter(IR_OUT_CARRIER_TIMER);
    LL_TIM_EnableCounter(IR_OUT_SIGNAL_TIMER);
}

void ir_tx_next_signal() {
    if (ir_tx_signal_idx >= ir_tx_signal_length) {
        ir_tx_signal_idx = 0;
        LL_TIM_DisableCounter(IR_OUT_CARRIER_TIMER);
        LL_TIM_DisableCounter(IR_OUT_SIGNAL_TIMER);
        ir_tx_enable_gpio(false);
        return;
    }

    uint32_t on_t = __LL_TIM_CALC_DELAY(SystemCoreClock, IR_OUT_SIGNAL_PRESCALER, ir_tx_signal[ir_tx_signal_idx]);
    uint32_t off_t = __LL_TIM_CALC_DELAY(SystemCoreClock, IR_OUT_SIGNAL_PRESCALER, ir_tx_signal[ir_tx_signal_idx + 1]);
    uint32_t total_t = on_t + off_t;

    LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, total_t);
    IR_OUT_SIGNAL_TIM_OC_SetCompare(on_t);
    ir_tx_signal_idx += 2;
}

void ir_tx_irq() {
    if (LL_TIM_IsActiveFlag_UPDATE(IR_OUT_SIGNAL_TIMER)) {
        LL_TIM_ClearFlag_UPDATE(IR_OUT_SIGNAL_TIMER);
        ir_tx_next_signal();
    }
}

static void ir_tx_enable_gpio(bool enable) {
    LL_GPIO_SetPinMode(IR_OUT_PORT, IR_OUT_PIN, enable ? LL_GPIO_MODE_ALTERNATE : LL_GPIO_MODE_OUTPUT);
    if (enable) {
        LL_GPIO_SetOutputPin(IR_OUT_PORT, IR_OUT_PIN);
    } else {
        LL_GPIO_ResetOutputPin(IR_OUT_PORT, IR_OUT_PIN);
    }
}