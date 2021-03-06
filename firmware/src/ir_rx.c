#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "ir_rx.h"
#include "time.h"

typedef uint16_t tim_value;

static tim_value ir_rx_dma_data[IR_RX_BUFFER_SAMPLES];
static size_t ir_rx_dma_data_read_index = 0;

volatile static size_t ir_rx_get_pos();

void ir_rx_setup() {
    uint32_t prescaler = __LL_TIM_CALC_PSC(SystemCoreClock, 1000000);
    LL_TIM_SetPrescaler(IR_RX_TIMER, prescaler);

    // init dma
    LL_DMA_EnableIT_TC(IR_RX_DMA, IR_RX_DMA_CH);
    LL_DMA_EnableIT_TE(IR_RX_DMA, IR_RX_DMA_CH);

    // start dma
    LL_DMA_DisableChannel(IR_RX_DMA, IR_RX_DMA_CH);
    IR_RX_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(IR_RX_DMA, IR_RX_DMA_CH, IR_RX_BUFFER_SAMPLES);
    LL_DMA_SetPeriphAddress(IR_RX_DMA, IR_RX_DMA_CH, (uint32_t) IR_RX_CAPTURE_REG_ADDR);
    LL_DMA_SetMemoryAddress(IR_RX_DMA, IR_RX_DMA_CH, (uint32_t) ir_rx_dma_data);
    LL_DMA_SetMemorySize(IR_RX_DMA, IR_RX_DMA_CH, LL_DMA_MDATAALIGN_HALFWORD);
    LL_DMA_EnableChannel(IR_RX_DMA, IR_RX_DMA_CH);

    memset(ir_rx_dma_data, 0, sizeof(ir_rx_dma_data));
    ir_rx_dma_data_read_index = ir_rx_get_pos();

    IR_RX_LL_TIM_EnableIT_CC();
    IR_RX_LL_TIM_EnableDMAReq_CC();
    LL_TIM_CC_EnableChannel(IR_RX_TIMER, IR_RX_TIMER_CH);
    LL_TIM_EnableCounter(IR_RX_TIMER);
}

void ir_rx_loop() {
    static tim_value last_value = 0;
    static uint32_t last_tick = 0;

    while (ir_rx_get_pos() != ir_rx_dma_data_read_index) {
        uint32_t value = ir_rx_dma_data[ir_rx_dma_data_read_index++];
        if (ir_rx_dma_data_read_index >= IR_RX_BUFFER_SAMPLES) {
            ir_rx_dma_data_read_index = 0;
        }

        tim_value t = value - last_value;
        uint32_t current_tick = time_get();
        if ((current_tick - last_tick) < 1000) { // prevent timer overflows
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