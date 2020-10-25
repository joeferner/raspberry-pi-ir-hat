#include "config.h"
#include "ir_rx.h"
#include "lwrb/lwrb.h"

static ir_rx_value ir_rx_dma_data[IR_RX_BUFFER_SAMPLES];
static size_t ir_rx_dma_data_read_index = 0;

void uart_start_tx_dma_transfer();

size_t ir_rx_get_pos();

void ir_rx_setup() {
    memset(ir_rx_dma_data, 0, sizeof(ir_rx_dma_data));
    ir_rx_dma_data_read_index = ir_rx_get_pos();
    HAL_StatusTypeDef res = HAL_TIM_IC_Start_DMA(&IR_RX_TIMER, IR_RX_CH, ir_rx_dma_data, sizeof(ir_rx_dma_data) / sizeof(ir_rx_value));
    if (res != HAL_OK) {
        main_error_handler();
    }
}

void ir_rx_loop() {
    static ir_rx_value last_value = 0;
    static uint32_t last_tick = 0;

    size_t pos = ir_rx_get_pos();
    while (pos != ir_rx_dma_data_read_index) {
        ir_rx_value value = ir_rx_dma_data[ir_rx_dma_data_read_index++];
        if (ir_rx_dma_data_read_index >= IR_RX_BUFFER_SAMPLES) {
            ir_rx_dma_data_read_index = 0;
        }

        uint32_t t = value - last_value;
        uint32_t current_tick = HAL_GetTick();
        if ((current_tick - last_tick) < 1000) { // prevent timer overflows
            ir_rx_received(t);
        }
        last_value = value;
        last_tick = current_tick;
    }
}

size_t ir_rx_get_pos() {
    return (IR_RX_BUFFER_SAMPLES - __HAL_DMA_GET_COUNTER(IR_RX_TIMER.hdma[TIM_DMA_ID_CC1])) % IR_RX_BUFFER_SAMPLES;
}
