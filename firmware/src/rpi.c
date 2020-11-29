#include "config.h"
#include "rpi.h"
#include "dma_tx_ring_buffer.h"
#include "dma_rx_ring_buffer.h"

// 0x7e
#define PACKET_BEGIN  0b01111110u
// 0x7f
#define PACKET_END    0b01111111u
// 0x7d
#define PACKET_ESCAPE 0b01111101u
// 0xdf
#define PACKET_MASK   0b11011111u

#define RX_BUFFER_LEN 512

static dma_tx_ring_buffer uart_tx_dma_ring_buffer;
static uint8_t uart_tx_dma_ring_buffer_data[256] __attribute__ ((aligned (8)));

static dma_rx_ring_buffer uart_rx_dma_ring_buffer;
static uint8_t uart_rx_dma_ring_buffer_data[RX_BUFFER_LEN] __attribute__ ((aligned (8)));

static void rpi_dma_tx_complete();

static void rpi_tx_begin_dma(struct dma_tx_ring_buffer_t *rb);

static size_t decode_buffer(uint8_t *buffer, size_t buffer_len, size_t *decoded_size);

void rpi_setup() {
    // USART DMA TX Init
    LL_DMA_EnableIT_TC(RPI_TX_DMA, RPI_TX_DMA_CH);
    LL_DMA_EnableIT_TE(RPI_TX_DMA, RPI_TX_DMA_CH);

    // ring buffers init
    dma_tx_ring_buffer_init(
        &uart_tx_dma_ring_buffer,
        uart_tx_dma_ring_buffer_data,
        sizeof(uart_tx_dma_ring_buffer_data),
        RPI_TX_DMA,
        RPI_TX_DMA_CH,
        rpi_tx_begin_dma
    );
    dma_rx_ring_buffer_init(
        &uart_rx_dma_ring_buffer,
        uart_rx_dma_ring_buffer_data,
        sizeof(uart_rx_dma_ring_buffer_data),
        RPI_RX_DMA,
        RPI_RX_DMA_CH
    );

    // begin rx dma
    LL_DMA_DisableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
    RPI_LL_DMA_ClearFlag_RX_GI();
    LL_DMA_SetDataLength(RPI_RX_DMA, RPI_RX_DMA_CH, sizeof(uart_rx_dma_ring_buffer_data));
    LL_DMA_SetPeriphAddress(RPI_RX_DMA, RPI_RX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART, LL_USART_DMA_REG_DATA_RECEIVE));
    LL_DMA_SetMemoryAddress(RPI_RX_DMA, RPI_RX_DMA_CH, (uint32_t) uart_rx_dma_ring_buffer_data);
    LL_DMA_EnableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
    LL_USART_EnableDMAReq_RX(RPI_USART);
}

void rpi_loop() {
    uint8_t b;
    while (dma_rx_ring_buffer_peek1(&uart_rx_dma_ring_buffer, &b)) {
        if (b == PACKET_BEGIN) {
            uint8_t peek_buffer[RX_BUFFER_LEN];
            size_t peek_size = dma_rx_ring_buffer_peek(&uart_rx_dma_ring_buffer, 0, peek_buffer, sizeof(peek_buffer));
            size_t decoded_size;
            size_t encoded_packet_size = decode_buffer(peek_buffer, peek_size, &decoded_size);
            if (encoded_packet_size > 0) {
                rpi_packet_header *header = (rpi_packet_header *) peek_buffer;
                if (RPI_PACKET_IS_VALID_TYPE(header->type) && header->size == decoded_size) {
                    rpi_rx(header);
                    dma_rx_ring_buffer_skip(&uart_rx_dma_ring_buffer, encoded_packet_size);
                } else {
                    dma_rx_ring_buffer_skip(&uart_rx_dma_ring_buffer, 1);
                }
            }
            break;
        } else {
            dma_rx_ring_buffer_skip(&uart_rx_dma_ring_buffer, 1);
        }
    }
}

void rpi_tx(const uint8_t *data, size_t data_len) {
    dma_tx_ring_buffer_write(&uart_tx_dma_ring_buffer, data, data_len);
}

void rpi_dma_irq() {
    if (RPI_LL_DMA_IsActiveFlag_TX_TC()) {
        RPI_LL_DMA_ClearFlag_TX_GI();
        rpi_dma_tx_complete();
    }

    if (RPI_LL_DMA_IsActiveFlag_RX_TC()) {
        RPI_LL_DMA_ClearFlag_RX_GI();
    }

    if (RPI_LL_DMA_IsActiveFlag_TX_GI()) {
        RPI_LL_DMA_ClearFlag_TX_GI();
    }

    if (RPI_LL_DMA_IsActiveFlag_RX_GI()) {
        RPI_LL_DMA_ClearFlag_RX_GI();
    }
}

void rpi_dma_tx_complete() {
    dma_tx_ring_buffer_irq(&uart_tx_dma_ring_buffer);
}

static void rpi_tx_begin_dma(struct dma_tx_ring_buffer_t *rb) {
    LL_DMA_SetPeriphAddress(RPI_TX_DMA, RPI_TX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART, LL_USART_DMA_REG_DATA_TRANSMIT));
    RPI_LL_DMA_ClearFlag_TX_GI();
    LL_USART_ClearFlag_TC(RPI_USART);
    LL_USART_EnableDMAReq_TX(RPI_USART);
}

static size_t decode_buffer(uint8_t *buffer, size_t buffer_len, size_t *decoded_size) {
    uint8_t *read_ptr = buffer;
    uint8_t *write_ptr = buffer;
    uint8_t *end = buffer + buffer_len;

    // begin
    if (read_ptr > end || *read_ptr != PACKET_BEGIN) {
        return 0;
    }
    *write_ptr++ = *read_ptr++;

    // body
    bool in_escape = false;
    while (read_ptr < end) {
        uint8_t b = *read_ptr;
        if (b == PACKET_END) {
            break;
        } else if (b == PACKET_ESCAPE) {
            in_escape = true;
            read_ptr++;
        } else {
            if (in_escape) {
                b = b | ~PACKET_MASK;
                in_escape = false;
            }
            *write_ptr++ = b;
            read_ptr++;
        }
    }

    // end
    if (read_ptr > end || *read_ptr != PACKET_END) {
        return 0;
    }
    *write_ptr++ = *read_ptr++;

    *decoded_size = write_ptr - buffer;
    return read_ptr - buffer;
}