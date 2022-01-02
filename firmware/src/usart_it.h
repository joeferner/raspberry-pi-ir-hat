#ifndef _usart_it_h_
#define _usart_it_h_

#include <stdint.h>

#include "main.h"
#include "uint8_ring_buffer.h"

typedef struct {
  USART_TypeDef *usart;
  uint8_ring_buffer tx_buffer;
  uint8_ring_buffer rx_buffer;
} usart_it;

void usart_it_init(usart_it *it, USART_TypeDef *usart, uint8_t *tx_buffer, size_t tx_buffer_length, uint8_t *rx_buffer,
                   size_t rx_buffer_length);

void usart_it_tx(usart_it *it, const uint8_t *data, size_t data_len);

size_t usart_it_rx_peek(usart_it *it, uint32_t offset, uint8_t *buffer, size_t read_len);

void usart_it_rx_skip(usart_it *it, size_t skip);

void usart_irq(usart_it *it);

#endif
