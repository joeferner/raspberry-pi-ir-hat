#include <stdlib.h>
#include "config.h"
#include "debug.h"
#include "lwrb/lwrb.h"

static volatile lwrb_t uart_tx_dma_ringbuff;
static uint8_t uart_tx_dma_lwrb_data[256] __attribute__ ((aligned (8)));;
static volatile size_t uart_tx_dma_current_len;

static volatile lwrb_t uart_rx_dma_ringbuff;
static uint8_t uart_rx_dma_lwrb_data[256] __attribute__ ((aligned (8)));;
static int uart_rx_old_pos = 0;

static void _debug_start_tx_dma_transfer();

static void _debug_dma_tx_complete();

static void _debug_dma_rx_complete();

static size_t _debug_rx_get_pos();

void debug_setup() {
    // USART DMA TX Init
    LL_DMA_EnableIT_TC(DEBUG_TX_DMA, DEBUG_TX_DMA_CH);
    LL_DMA_EnableIT_TE(DEBUG_TX_DMA, DEBUG_TX_DMA_CH);

    // ring buffers init
    lwrb_init(&uart_tx_dma_ringbuff, uart_tx_dma_lwrb_data, sizeof(uart_tx_dma_lwrb_data));
    lwrb_init(&uart_rx_dma_ringbuff, uart_rx_dma_lwrb_data, sizeof(uart_rx_dma_lwrb_data));
    uart_tx_dma_current_len = 0;

    // begin rx dma
    LL_DMA_DisableChannel(DEBUG_RX_DMA, DEBUG_RX_DMA_CH);
    DEBUG_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(DEBUG_RX_DMA, DEBUG_RX_DMA_CH, sizeof(uart_rx_dma_lwrb_data));
    LL_DMA_SetPeriphAddress(DEBUG_RX_DMA, DEBUG_RX_DMA_CH, LL_USART_DMA_GetRegAddr(DEBUG_USART, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetMemoryAddress(DEBUG_RX_DMA, DEBUG_RX_DMA_CH, (uint32_t) uart_rx_dma_lwrb_data);
    LL_DMA_EnableChannel(DEBUG_RX_DMA, DEBUG_RX_DMA_CH);
    LL_USART_EnableDMAReq_RX(DEBUG_USART);

    uart_rx_old_pos = _debug_rx_get_pos();
}

void debug_loop() {
    _debug_dma_rx_complete();

    uint8_t peek_buffer[256];
    size_t peek_size;
    peek_size = lwrb_peek(&uart_rx_dma_ringbuff, 0, peek_buffer, sizeof(peek_buffer));
    for (size_t i = 0; i < peek_size; i++) {
        if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
            i++;
            peek_buffer[i] = '\0';
            lwrb_skip(&uart_rx_dma_ringbuff, i);
            debug_rx(peek_buffer, i);
            break;
        }
    }
}

/**
 * \brief do not call this in an interrupt
 */
void debug_send_string(const char *str) {
    debug_tx((const uint8_t *) str, strlen(str));
}

/**
 * \brief do not call this in an interrupt
 */
void debug_send_uint32(uint32_t value) {
    char buffer[15];
    utoa(value, buffer, 10);
    debug_send_string(buffer);
}

/**
 * \brief do not call this in an interrupt
 */
void debug_tx(const uint8_t *data, size_t data_len) {
    if (data_len > sizeof(uart_tx_dma_lwrb_data)) {
        Error_Handler();
    }
    while (lwrb_get_free(&uart_tx_dma_ringbuff) < data_len) {
        _debug_start_tx_dma_transfer();
    }
    lwrb_write(&uart_tx_dma_ringbuff, data, data_len);
    _debug_start_tx_dma_transfer();
}

void _debug_start_tx_dma_transfer() {
    if (uart_tx_dma_current_len == 0) {
        uart_tx_dma_current_len = lwrb_get_linear_block_read_length(&uart_tx_dma_ringbuff);
        if (uart_tx_dma_current_len > 0) {
            void *p = lwrb_get_linear_block_read_address(&uart_tx_dma_ringbuff);

            LL_DMA_DisableChannel(DEBUG_TX_DMA, DEBUG_TX_DMA_CH);
            DEBUG_LL_DMA_ClearFlag_TX_GI();
            LL_DMA_SetDataLength(DEBUG_TX_DMA, DEBUG_TX_DMA_CH, uart_tx_dma_current_len);
            LL_DMA_SetPeriphAddress(DEBUG_TX_DMA, DEBUG_TX_DMA_CH, LL_USART_DMA_GetRegAddr(DEBUG_USART, LL_USART_DMA_REG_DATA_TRANSMIT));
            LL_DMA_SetMemoryAddress(DEBUG_TX_DMA, DEBUG_TX_DMA_CH, (uint32_t) p);
            LL_DMA_EnableChannel(DEBUG_TX_DMA, DEBUG_TX_DMA_CH);
            LL_USART_ClearFlag_TC(DEBUG_USART);
            LL_USART_EnableDMAReq_TX(DEBUG_USART);
        }
    }
}

void debug_dma_irq() {
    if (DEBUG_LL_DMA_IsActiveFlag_TX_TC()) {
        DEBUG_LL_DMA_ClearFlag_TX_GI();
        _debug_dma_tx_complete();
    }

    if (DEBUG_LL_DMA_IsActiveFlag_RX_TC()) {
        DEBUG_LL_DMA_ClearFlag_RX_GI();
        _debug_dma_rx_complete();
    }

    if (DEBUG_LL_DMA_IsActiveFlag_TX_GI()) {
        DEBUG_LL_DMA_ClearFlag_TX_GI();
    }

    if (DEBUG_LL_DMA_IsActiveFlag_RX_GI()) {
        DEBUG_LL_DMA_ClearFlag_RX_GI();
    }
}

void _debug_dma_tx_complete() {
    lwrb_skip(&uart_tx_dma_ringbuff, uart_tx_dma_current_len);
    uart_tx_dma_current_len = 0;
    _debug_start_tx_dma_transfer();
}

void _debug_dma_rx_complete() {
    size_t pos = _debug_rx_get_pos();
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

size_t _debug_rx_get_pos() {
    return (sizeof(uart_rx_dma_lwrb_data) - LL_DMA_GetDataLength(DEBUG_RX_DMA, DEBUG_RX_DMA_CH));
}
