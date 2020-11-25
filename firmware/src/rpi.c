#include <stdlib.h>
#include "config.h"
#include "rpi.h"
#include "lwrb/lwrb.h"

static volatile lwrb_t uart_tx_dma_ringbuff;
static uint8_t uart_tx_dma_lwrb_data[256] __attribute__ ((aligned (8)));;
static volatile size_t uart_tx_dma_current_len;

static volatile lwrb_t uart_rx_dma_ringbuff;
static uint8_t uart_rx_dma_lwrb_data[256] __attribute__ ((aligned (8)));;
static int uart_rx_old_pos = 0;

static void _rpi_start_tx_dma_transfer();

static void _rpi_dma_tx_complete();

static void _rpi_dma_rx_complete();

static size_t _rpi_rx_get_pos();

void rpi_setup() {
    // USART DMA TX Init
    LL_DMA_EnableIT_TC(RPI_TX_DMA, RPI_TX_DMA_CH);
    LL_DMA_EnableIT_TE(RPI_TX_DMA, RPI_TX_DMA_CH);

    // ring buffers init
    lwrb_init(&uart_tx_dma_ringbuff, uart_tx_dma_lwrb_data, sizeof(uart_tx_dma_lwrb_data));
    lwrb_init(&uart_rx_dma_ringbuff, uart_rx_dma_lwrb_data, sizeof(uart_rx_dma_lwrb_data));
    uart_tx_dma_current_len = 0;

    // begin rx dma
    LL_DMA_DisableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
    RPI_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(RPI_RX_DMA, RPI_RX_DMA_CH, sizeof(uart_rx_dma_lwrb_data));
    LL_DMA_SetPeriphAddress(RPI_RX_DMA, RPI_RX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetMemoryAddress(RPI_RX_DMA, RPI_RX_DMA_CH, (uint32_t) uart_rx_dma_lwrb_data);
    LL_DMA_EnableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
    LL_USART_EnableDMAReq_RX(RPI_USART);

    uart_rx_old_pos = _rpi_rx_get_pos();
}

void rpi_loop() {
    _rpi_dma_rx_complete();

    uint8_t peek_buffer[256];
    size_t peek_size;
    peek_size = lwrb_peek(&uart_rx_dma_ringbuff, 0, peek_buffer, sizeof(peek_buffer));
    for (size_t i = 0; i < peek_size; i++) {
        if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
            i++;
            peek_buffer[i] = '\0';
            lwrb_skip(&uart_rx_dma_ringbuff, i);
            rpi_rx(peek_buffer, i);
            break;
        }
    }
}

/**
 * \brief do not call this in an interrupt
 */
void rpi_send_string(const char *str) {
    rpi_tx((const uint8_t *) str, strlen(str));
}

/**
 * \brief do not call this in an interrupt
 */
void rpi_send_uint32(uint32_t value) {
    char buffer[15];
    utoa(value, buffer, 10);
    rpi_send_string(buffer);
}

/**
 * \brief do not call this in an interrupt
 */
void rpi_tx(const uint8_t *data, size_t data_len) {
    if (data_len > sizeof(uart_tx_dma_lwrb_data)) {
        Error_Handler();
    }
    while (lwrb_get_free(&uart_tx_dma_ringbuff) < data_len) {
        _rpi_start_tx_dma_transfer();
    }
    lwrb_write(&uart_tx_dma_ringbuff, data, data_len);
    _rpi_start_tx_dma_transfer();
}

void _rpi_start_tx_dma_transfer() {
    if (uart_tx_dma_current_len == 0) {
        uart_tx_dma_current_len = lwrb_get_linear_block_read_length(&uart_tx_dma_ringbuff);
        if (uart_tx_dma_current_len > 0) {
            void *p = lwrb_get_linear_block_read_address(&uart_tx_dma_ringbuff);

            LL_DMA_DisableChannel(RPI_TX_DMA, RPI_TX_DMA_CH);
            RPI_LL_DMA_ClearFlag_TX_GI();
            LL_DMA_SetDataLength(RPI_TX_DMA, RPI_TX_DMA_CH, uart_tx_dma_current_len);
            LL_DMA_SetPeriphAddress(RPI_TX_DMA, RPI_TX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART, LL_USART_DMA_REG_DATA_TRANSMIT));
            LL_DMA_SetMemoryAddress(RPI_TX_DMA, RPI_TX_DMA_CH, (uint32_t) p);
            LL_DMA_EnableChannel(RPI_TX_DMA, RPI_TX_DMA_CH);
            LL_USART_ClearFlag_TC(RPI_USART);
            LL_USART_EnableDMAReq_TX(RPI_USART);
        }
    }
}

void rpi_dma_irq() {
    if (RPI_LL_DMA_IsActiveFlag_TX_TC()) {
        RPI_LL_DMA_ClearFlag_TX_GI();
        _rpi_dma_tx_complete();
    }

    if (RPI_LL_DMA_IsActiveFlag_RX_TC()) {
        RPI_LL_DMA_ClearFlag_RX_GI();
        _rpi_dma_rx_complete();
    }

    if (RPI_LL_DMA_IsActiveFlag_TX_GI()) {
        RPI_LL_DMA_ClearFlag_TX_GI();
    }

    if (RPI_LL_DMA_IsActiveFlag_RX_GI()) {
        RPI_LL_DMA_ClearFlag_RX_GI();
    }
}

void _rpi_dma_tx_complete() {
    lwrb_skip(&uart_tx_dma_ringbuff, uart_tx_dma_current_len);
    uart_tx_dma_current_len = 0;
    _rpi_start_tx_dma_transfer();
}

void _rpi_dma_rx_complete() {
    size_t pos = _rpi_rx_get_pos();
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

size_t _rpi_rx_get_pos() {
    return (sizeof(uart_rx_dma_lwrb_data) - LL_DMA_GetDataLength(RPI_RX_DMA, RPI_RX_DMA_CH));
}
