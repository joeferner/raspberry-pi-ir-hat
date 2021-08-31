#include "rpi.h"

#include <stdlib.h>
#include <string.h>

#include "config.h"

// #define RX_BUFFER_LEN RPI_RX_BUFFER_LEN
// #define TX_BUFFER_LEN RPI_TX_BUFFER_LEN

// static uint8_ring_buffer uart_tx_dma_ring_buffer;
// static uint8_t uart_tx_dma_ring_buffer_data[TX_BUFFER_LEN] __attribute__((aligned(8)));

// static dma_rx_ring_buffer uart_rx_dma_ring_buffer;
// static uint8_t uart_rx_dma_ring_buffer_data[RX_BUFFER_LEN] __attribute__((aligned(8)));

// void rpi_setup() {
//   // ring buffers init
//   uint8_ring_buffer_init(&uart_tx_dma_ring_buffer, uart_tx_dma_ring_buffer_data,
//   sizeof(uart_tx_dma_ring_buffer_data)); dma_rx_ring_buffer_init(&uart_rx_dma_ring_buffer,
//   uart_rx_dma_ring_buffer_data, sizeof(uart_rx_dma_ring_buffer_data),
//                           RPI_RX_DMA, RPI_RX_DMA_CH);

//   // begin rx dma
//   LL_DMA_DisableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
//   RPI_LL_DMA_ClearFlag_RX_GI();
//   LL_DMA_SetDataLength(RPI_RX_DMA, RPI_RX_DMA_CH, sizeof(uart_rx_dma_ring_buffer_data));
//   LL_DMA_SetPeriphAddress(RPI_RX_DMA, RPI_RX_DMA_CH, LL_USART_DMA_GetRegAddr(RPI_USART,
//   LL_USART_DMA_REG_DATA_RECEIVE)); LL_DMA_SetMemoryAddress(RPI_RX_DMA, RPI_RX_DMA_CH,
//   (uint32_t)uart_rx_dma_ring_buffer_data); LL_DMA_EnableChannel(RPI_RX_DMA, RPI_RX_DMA_CH);
//   LL_USART_EnableDMAReq_RX(RPI_USART);

//   // enable tx irq
//   LL_USART_EnableIT_TXE_TXFNF(RPI_USART);
// }

// void rpi_loop() {
//   uint8_t peek_buffer[RX_BUFFER_LEN + 1];
//   size_t peek_size;
//   peek_size = dma_rx_ring_buffer_peek(&uart_rx_dma_ring_buffer, 0, peek_buffer, sizeof(peek_buffer));
//   for (size_t i = 0; i < peek_size; i++) {
//     if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
//       peek_buffer[i] = '\0';
//       dma_rx_ring_buffer_skip(&uart_rx_dma_ring_buffer, i + 1);
//       rpi_rx((char *)peek_buffer);
//       break;
//     }
//   }
// }

// void rpi_send_string(const char *str) { rpi_tx((const uint8_t *)str, strlen(str)); }

// void rpi_send_uint32(uint32_t value) {
//   char buffer[15];
//   utoa(value, buffer, 10);
//   rpi_send_string(buffer);
// }

// void rpi_tx(const uint8_t *data, size_t data_len) {
//   for (size_t i = 0; i < data_len; i++) {
//     uint8_ring_buffer_write(&uart_tx_dma_ring_buffer, data[i]);
//   }

//   if (uint8_ring_buffer_length(&uart_tx_dma_ring_buffer) == 0) {
//     LL_USART_TransmitData8(RPI_USART, uint8_ring_buffer_read(&uart_tx_dma_ring_buffer));
//   }
// }

// void rpi_dma_irq() {
//   if (RPI_LL_DMA_IsActiveFlag_RX_TC()) {
//     RPI_LL_DMA_ClearFlag_RX_GI();
//   }

//   if (RPI_LL_DMA_IsActiveFlag_RX_GI()) {
//     RPI_LL_DMA_ClearFlag_RX_GI();
//   }
// }

void rpi_usart_irq() {
  //   if (LL_USART_IsEnabledIT_TXE_TXFNF(RPI_USART) && LL_USART_IsActiveFlag_TXE_TXFNF(RPI_USART)) {
  //     LL_USART_TransmitData8(RPI_USART, uint8_ring_buffer_read(&uart_tx_dma_ring_buffer));
  //   }

  //   if (LL_USART_IsEnabledIT_TC(RPI_USART) && LL_USART_IsActiveFlag_TC(RPI_USART)) {
  //     LL_USART_ClearFlag_TC(RPI_USART);
  //   }

  //   if (LL_USART_IsEnabledIT_ERROR(RPI_USART) && LL_USART_IsActiveFlag_NE(RPI_USART)) {
  //     while (1)
  //       ;
  //   }
}
