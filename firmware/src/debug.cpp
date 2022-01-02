#include "debug.hpp"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "usart_it.hpp"

#define RX_BUFFER_LEN DEBUG_RX_BUFFER_LEN
#define TX_BUFFER_LEN DEBUG_TX_BUFFER_LEN

static usart_it usart;

void debug_send_string(const char *str) {
  debug_tx((const uint8_t *)str, strlen(str));
}

void debug_send_uint32(uint32_t value) {
  char buffer[15];
  utoa(value, buffer, 10);
  debug_send_string(buffer);
}

void debug_tx(const uint8_t *data, size_t data_len) {
  usart_it_tx(&usart, data, data_len);
}

void debug_usart_irq() {
  usart_irq(&usart);
}
