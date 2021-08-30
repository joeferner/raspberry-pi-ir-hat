#include "debug.h"

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "dma_rx_ring_buffer.h"
#include "dma_tx_ring_buffer.h"

#define RX_BUFFER_LEN DEBUG_RX_BUFFER_LEN
#define TX_BUFFER_LEN DEBUG_TX_BUFFER_LEN

static dma_tx_ring_buffer uart_tx_dma_ring_buffer;
static uint8_t uart_tx_dma_ring_buffer_data[TX_BUFFER_LEN] __attribute__((aligned(8)));

static dma_rx_ring_buffer uart_rx_dma_ring_buffer;
static uint8_t uart_rx_dma_ring_buffer_data[RX_BUFFER_LEN] __attribute__((aligned(8)));

static void debug_dma_tx_complete();

static void debug_tx_begin_dma(struct dma_tx_ring_buffer_t *rb);

void debug_setup() {
  // USART DMA TX Init
  LL_DMA_EnableIT_TC(DEBUG_TX_DMA, DEBUG_TX_DMA_CH);
  LL_DMA_EnableIT_TE(DEBUG_TX_DMA, DEBUG_TX_DMA_CH);

  // ring buffers init
  dma_tx_ring_buffer_init(&uart_tx_dma_ring_buffer, uart_tx_dma_ring_buffer_data, sizeof(uart_tx_dma_ring_buffer_data),
                          DEBUG_TX_DMA, DEBUG_TX_DMA_CH, debug_tx_begin_dma);
  dma_rx_ring_buffer_init(&uart_rx_dma_ring_buffer, uart_rx_dma_ring_buffer_data, sizeof(uart_rx_dma_ring_buffer_data),
                          DEBUG_RX_DMA, DEBUG_RX_DMA_CH);

  // begin rx dma
  LL_DMA_DisableChannel(DEBUG_RX_DMA, DEBUG_RX_DMA_CH);
  DEBUG_LL_DMA_ClearFlag_RX_GI();
  LL_DMA_SetDataLength(DEBUG_RX_DMA, DEBUG_RX_DMA_CH, sizeof(uart_rx_dma_ring_buffer_data));
  LL_DMA_SetPeriphAddress(DEBUG_RX_DMA, DEBUG_RX_DMA_CH,
                          LL_USART_DMA_GetRegAddr(DEBUG_USART, LL_USART_DMA_REG_DATA_RECEIVE));
  LL_DMA_SetMemoryAddress(DEBUG_RX_DMA, DEBUG_RX_DMA_CH, (uint32_t)uart_rx_dma_ring_buffer_data);
  LL_DMA_EnableChannel(DEBUG_RX_DMA, DEBUG_RX_DMA_CH);
  LL_USART_EnableDMAReq_RX(DEBUG_USART);
}

void debug_loop() {
  uint8_t peek_buffer[RX_BUFFER_LEN + 1];
  size_t peek_size;
  peek_size = dma_rx_ring_buffer_peek(&uart_rx_dma_ring_buffer, 0, peek_buffer, sizeof(peek_buffer));
  for (size_t i = 0; i < peek_size; i++) {
    if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
      peek_buffer[i] = '\0';
      dma_rx_ring_buffer_skip(&uart_rx_dma_ring_buffer, i + 1);
      debug_rx((char *)peek_buffer);
      break;
    }
  }
}

void debug_send_string(const char *str) { debug_tx((const uint8_t *)str, strlen(str)); }

void debug_send_uint32(uint32_t value) {
  char buffer[15];
  utoa(value, buffer, 10);
  debug_send_string(buffer);
}

void debug_tx(const uint8_t *data, size_t data_len) {
  dma_tx_ring_buffer_write(&uart_tx_dma_ring_buffer, data, data_len);
}

void debug_dma_irq() {
  if (DEBUG_LL_DMA_IsActiveFlag_TX_TC()) {
    DEBUG_LL_DMA_ClearFlag_TX_GI();
    debug_dma_tx_complete();
  }

  if (DEBUG_LL_DMA_IsActiveFlag_RX_TC()) {
    DEBUG_LL_DMA_ClearFlag_RX_GI();
  }

  if (DEBUG_LL_DMA_IsActiveFlag_TX_GI()) {
    DEBUG_LL_DMA_ClearFlag_TX_GI();
  }

  if (DEBUG_LL_DMA_IsActiveFlag_RX_GI()) {
    DEBUG_LL_DMA_ClearFlag_RX_GI();
  }
}

void debug_dma_tx_complete() { dma_tx_ring_buffer_irq(&uart_tx_dma_ring_buffer); }

void debug_tx_begin_dma(struct dma_tx_ring_buffer_t *rb) {
  LL_DMA_SetPeriphAddress(DEBUG_TX_DMA, DEBUG_TX_DMA_CH,
                          LL_USART_DMA_GetRegAddr(DEBUG_USART, LL_USART_DMA_REG_DATA_TRANSMIT));
  DEBUG_LL_DMA_ClearFlag_TX_GI();
  LL_USART_ClearFlag_TC(DEBUG_USART);
  LL_USART_EnableDMAReq_TX(DEBUG_USART);
}
