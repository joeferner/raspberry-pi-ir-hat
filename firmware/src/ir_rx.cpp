#include "ir_rx.hpp"

#include <stdlib.h>
#include <string.h>

#include "globals.hpp"
#include "main.h"

typedef uint16_t tim_value;

static tim_value ir_rx_dma_data[IR_RX_BUFFER_SAMPLES];
static size_t ir_rx_dma_data_read_index = 0;

volatile static size_t ir_rx_get_pos();

void ir_rx_loop() {
  static tim_value last_value = 0;
  static uint32_t last_tick = 0;

  while (ir_rx_get_pos() != ir_rx_dma_data_read_index) {
    uint32_t value = ir_rx_dma_data[ir_rx_dma_data_read_index++];
    if (ir_rx_dma_data_read_index >= IR_RX_BUFFER_SAMPLES) {
      ir_rx_dma_data_read_index = 0;
    }

    tim_value t = value - last_value;
    uint32_t current_tick = clocks.getTickCount();
    if ((current_tick - last_tick) < 1000) {  // prevent timer overflows
      ir_rx_received(t);
    }
    last_value = value;
    last_tick = current_tick;
  }
}

volatile static size_t ir_rx_get_pos() {
  return (IR_RX_BUFFER_SAMPLES - LL_DMA_GetDataLength(IR_RX_DMA, IR_RX_DMA_CH));
}

void ir_rx_irq() {
  if (IR_RX_LL_DMA_IsActiveFlag_RX_TC()) {
    IR_RX_LL_DMA_ClearFlag_RX_GI();
  }

  if (IR_RX_LL_DMA_IsActiveFlag_RX_GI()) {
    IR_RX_LL_DMA_ClearFlag_RX_GI();
  }
}