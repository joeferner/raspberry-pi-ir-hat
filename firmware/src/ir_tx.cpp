#include "ir_tx.hpp"

#include <stdbool.h>

#include "globals.hpp"
#include "main.h"
#include "uint32_ring_buffer.hpp"

static uint32_t ir_tx_buffer[50];
static uint32_ring_buffer ir_tx_ring_buffer;
static bool ir_tx_sending;

static void ir_tx_stop();

static void ir_tx_next_signal();

static void ir_tx_enable_gpio(bool enable);

void ir_tx_setup() {
  ir_tx_enable_gpio(false);
  LL_SYSCFG_SetIRPolarity(LL_SYSCFG_IR_POL_INVERTED);

  uint32_ring_buffer_init(&ir_tx_ring_buffer, ir_tx_buffer, sizeof(ir_tx_buffer) / sizeof(uint32_t));
  ir_tx_sending = false;

  // Not sure why we need this but the first transmit is always wrong
  ir_tx_reset(38000);
  ir_tx_write(1000, 1000);
  ir_tx_send();
}

void ir_tx_reset(uint32_t carrier_freq) {
  ir_tx_stop();

  // init carrier timer
  uint32_t autoReload = __LL_TIM_CALC_ARR(SystemCoreClock, IR_OUT_CARRIER_PRESCALER, carrier_freq);
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

void ir_tx_write(const uint32_t t_on, const uint32_t t_off) {
  uint32_ring_buffer_write(&ir_tx_ring_buffer, t_on);
  uint32_ring_buffer_write(&ir_tx_ring_buffer, t_off);
}

void ir_tx_send() {
  if (!ir_tx_sending) {
    ir_tx_sending = true;

    ir_tx_next_signal();
    ir_tx_enable_gpio(true);
    LL_TIM_EnableCounter(IR_OUT_CARRIER_TIMER);
    LL_TIM_EnableCounter(IR_OUT_SIGNAL_TIMER);
  }
}

size_t ir_tx_buffer_length() {
  return uint32_ring_buffer_length(&ir_tx_ring_buffer);
}

void ir_tx_stop() {
  ir_tx_sending = false;
  LL_TIM_DisableCounter(IR_OUT_CARRIER_TIMER);
  LL_TIM_DisableCounter(IR_OUT_SIGNAL_TIMER);
  ir_tx_enable_gpio(false);
}

void ir_tx_next_signal() {
  if (uint32_ring_buffer_length(&ir_tx_ring_buffer) < 2) {
    ir_tx_stop();
    debugUsart.write("?send complete\n");
    return;
  }

  uint32_t t_on = uint32_ring_buffer_read(&ir_tx_ring_buffer);
  uint32_t t_off = uint32_ring_buffer_read(&ir_tx_ring_buffer);

  uint32_t on_t = __LL_TIM_CALC_DELAY(SystemCoreClock, IR_OUT_SIGNAL_PRESCALER, t_on);
  uint32_t off_t = __LL_TIM_CALC_DELAY(SystemCoreClock, IR_OUT_SIGNAL_PRESCALER, t_off);
  uint32_t total_t = on_t + off_t;

  LL_TIM_SetAutoReload(IR_OUT_SIGNAL_TIMER, total_t);
  IR_OUT_SIGNAL_TIM_OC_SetCompare(on_t);
}

void ir_tx_irq() {
  if (LL_TIM_IsActiveFlag_UPDATE(IR_OUT_SIGNAL_TIMER)) {
    LL_TIM_ClearFlag_UPDATE(IR_OUT_SIGNAL_TIMER);
    ir_tx_next_signal();
  }
}

static void ir_tx_enable_gpio(bool enable) {
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_9, enable ? LL_GPIO_MODE_ALTERNATE : LL_GPIO_MODE_OUTPUT);
  if (enable) {
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_9);
  } else {
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_9);
  }
}