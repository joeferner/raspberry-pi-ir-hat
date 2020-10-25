#include <stdlib.h>
#include "config.h"
#include "uart.h"
#include "lwrb/lwrb.h"

static lwrb_t uart_tx_dma_ringbuff;
static uint8_t uart_tx_dma_lwrb_data[256];
static volatile size_t uart_tx_dma_current_len;

static lwrb_t uart_rx_dma_ringbuff;
static uint8_t uart_rx_dma_lwrb_data[256];
static int uart_rx_old_pos = 0;

void uart_start_tx_dma_transfer();

#define uart_rx_get_pos() (sizeof(uart_rx_dma_lwrb_data) - __HAL_DMA_GET_COUNTER(COM_UART.hdmarx))

void uart_setup() {
    lwrb_init(&uart_tx_dma_ringbuff, uart_tx_dma_lwrb_data, sizeof(uart_tx_dma_lwrb_data));
    lwrb_init(&uart_rx_dma_ringbuff, uart_rx_dma_lwrb_data, sizeof(uart_rx_dma_lwrb_data));
    uart_tx_dma_current_len = 0;

    HAL_StatusTypeDef res = HAL_UART_Receive_DMA(&COM_UART, uart_rx_dma_lwrb_data, sizeof(uart_rx_dma_lwrb_data));
    if (res != HAL_OK) {
        main_error_handler();
    }

    uart_rx_old_pos = uart_rx_get_pos();
}

void uart_loop() {
    HAL_UART_RxCpltCallback(&COM_UART);

    uint8_t peek_buffer[256];
    size_t peek_size;
    peek_size = lwrb_peek(&uart_rx_dma_ringbuff, 0, peek_buffer, sizeof(peek_buffer));
    for (size_t i = 0; i < peek_size; i++) {
        if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
            i++;
            peek_buffer[i] = '\0';
            lwrb_skip(&uart_rx_dma_ringbuff, i);
            uart_rx(peek_buffer, i);
            break;
        }
    }
}

void uart_send_string(const char *str) {
    uart_tx((const uint8_t *) str, strlen(str));
}

void uart_send_uint32(uint32_t value) {
    char buffer[15];
    utoa(value, buffer, 10);
    uart_send_string(buffer);
}

void uart_tx(const uint8_t *data, size_t data_len) {
    if (data_len > sizeof(uart_tx_dma_lwrb_data)) {
        main_error_handler();
    }
    while (lwrb_get_free(&uart_tx_dma_ringbuff) < data_len);
    lwrb_write(&uart_tx_dma_ringbuff, data, data_len);
    uart_start_tx_dma_transfer();
}

void uart_start_tx_dma_transfer() {
    if (uart_tx_dma_current_len == 0) {
        uart_tx_dma_current_len = lwrb_get_linear_block_read_length(&uart_tx_dma_ringbuff);
        if (uart_tx_dma_current_len > 0) {
            void *p = lwrb_get_linear_block_read_address(&uart_tx_dma_ringbuff);
            HAL_StatusTypeDef res = HAL_UART_Transmit_DMA(&COM_UART, p, uart_tx_dma_current_len);
            if (res != HAL_OK) {
                main_error_handler();
            }
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    huart->gState = HAL_UART_STATE_READY;
    lwrb_skip(&uart_tx_dma_ringbuff, uart_tx_dma_current_len);
    uart_tx_dma_current_len = 0;
    uart_start_tx_dma_transfer();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    size_t pos = uart_rx_get_pos();
    if (pos != uart_rx_old_pos) {
        if (pos > uart_rx_old_pos) {
            // not wrapped
            lwrb_advance(&uart_rx_dma_ringbuff, pos - uart_rx_old_pos);
        } else {
            // wrapped
            lwrb_advance(&uart_rx_dma_ringbuff, (sizeof(uart_rx_dma_lwrb_data) - uart_rx_old_pos) + pos);
        }
    }
    uart_rx_old_pos = pos;
    if (uart_rx_old_pos == sizeof(uart_rx_dma_lwrb_data)) {
        uart_rx_old_pos = 0;
    }
}
