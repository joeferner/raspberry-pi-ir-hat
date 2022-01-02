#include "rpi.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "usart_it.h"

#define RX_BUFFER_LEN RPI_RX_BUFFER_LEN
#define TX_BUFFER_LEN RPI_TX_BUFFER_LEN

static usart_it usart;
static uint8_t usart_tx_buffer[TX_BUFFER_LEN];
static uint8_t usart_rx_buffer[RX_BUFFER_LEN];

void rpi_setup() {
  usart_it_init(&usart, RPI_USART, usart_tx_buffer, sizeof(usart_tx_buffer), usart_rx_buffer, sizeof(usart_rx_buffer));
}

void rpi_loop() {
  uint8_t peek_buffer[RX_BUFFER_LEN + 1];
  size_t peek_size;
  peek_size = usart_it_rx_peek(&usart, 0, peek_buffer, sizeof(peek_buffer));
  for (size_t i = 0; i < peek_size; i++) {
    if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
      peek_buffer[i] = '\0';
      usart_it_rx_skip(&usart, i + 1);
      rpi_rx((char *)peek_buffer);
      break;
    }
  }
}

void rpi_send_string(const char *str) {
  rpi_tx((const uint8_t *)str, strlen(str));
}

void rpi_send_uint32(uint32_t value) {
  char buffer[15];
  utoa(value, buffer, 10);
  rpi_send_string(buffer);
}

void rpi_tx(const uint8_t *data, size_t data_len) {
  usart_it_tx(&usart, data, data_len);
}

void rpi_usart_irq() {
  usart_irq(&usart);
}
