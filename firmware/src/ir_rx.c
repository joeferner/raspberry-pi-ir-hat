#include "config.h"
#include "ir_rx.h"
#include "lwrb/lwrb.h"

static lwrb_t ir_rx_dma_ringbuff;
static ir_rx_value ir_rx_dma_lwrb_data[IR_RX_BUFFER_SAMPLES * sizeof(ir_rx_value)];
static size_t ir_rx_old_pos = 0;

void uart_start_tx_dma_transfer();

size_t ir_rx_get_pos();

size_t round_down_to_nearest_4(size_t i);

void ir_rx_setup() {
    lwrb_init(&ir_rx_dma_ringbuff, ir_rx_dma_lwrb_data, sizeof(ir_rx_dma_lwrb_data));

    HAL_StatusTypeDef res = HAL_TIM_IC_Start_DMA(&IR_RX_TIMER, IR_RX_CH, ir_rx_dma_lwrb_data, sizeof(ir_rx_dma_lwrb_data) / sizeof(ir_rx_value));
    if (res != HAL_OK) {
        main_error_handler();
    }

    ir_rx_old_pos = ir_rx_get_pos();
}

void ir_rx_loop() {
    static ir_rx_value last_value = 0;
    HAL_TIM_IC_CaptureCallback(&IR_RX_TIMER);

    ir_rx_value peek_buffer[IR_RX_BUFFER_SAMPLES * sizeof(ir_rx_value)];
    int peek_size = lwrb_peek(&ir_rx_dma_ringbuff, 0, peek_buffer, sizeof(peek_buffer));
    if (peek_size > 0) {
        peek_size = round_down_to_nearest_4(peek_size);
        if (peek_size > 100) {
            peek_size++;
        }

        int peek_count = peek_size / sizeof(peek_buffer[0]);
        for (size_t i = 0; i < peek_count; i++) {
            ir_rx_value value = peek_buffer[i] - last_value;
            ir_rx_received(value);
            last_value = peek_buffer[i];
        }
        lwrb_skip(&ir_rx_dma_ringbuff, peek_size);
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    UNUSED(htim);

    size_t pos = ir_rx_get_pos();
    if (pos != ir_rx_old_pos) {
        uint32_t advance_amt;
        if (pos > ir_rx_old_pos) {
            // not wrapped
            advance_amt = (pos - ir_rx_old_pos) * sizeof(ir_rx_dma_lwrb_data[0]);
        } else {
            // wrapped
            advance_amt = ((sizeof(ir_rx_dma_lwrb_data) - ir_rx_old_pos) + pos) * sizeof(ir_rx_dma_lwrb_data[0]);
        }
        lwrb_advance(&ir_rx_dma_ringbuff, advance_amt);
        ir_rx_old_pos = pos;
        if (ir_rx_old_pos == sizeof(ir_rx_dma_lwrb_data)) {
            ir_rx_old_pos = 0;
        }
    }
}

size_t ir_rx_get_pos() {
    size_t raw_pos = sizeof(ir_rx_dma_lwrb_data) - __HAL_DMA_GET_COUNTER(IR_RX_TIMER.hdma[TIM_DMA_ID_CC1]);
    return round_down_to_nearest_4(raw_pos);
}

size_t round_down_to_nearest_4(size_t i) {
    return ((i >> 2) << 2);
}
