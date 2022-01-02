#include "usart_it.hpp"

#include "main.h"

// TODO remove
void usart_it_init(usart_it* it, USART_TypeDef* usart, uint8_t* tx_buffer, size_t tx_buffer_length, uint8_t* rx_buffer,
                   size_t rx_buffer_length) {
  it->usart = usart;
  uint8_ring_buffer_init(&it->tx_buffer, tx_buffer, tx_buffer_length);
  uint8_ring_buffer_init(&it->rx_buffer, rx_buffer, rx_buffer_length);

  LL_USART_EnableIT_RXNE_RXFNE(it->usart);
  LL_USART_EnableIT_ERROR(it->usart);
}

void usart_it_tx(usart_it* it, const uint8_t* data, size_t data_len) {
  for (size_t i = 0; i < data_len; i++) {
    if (uint8_ring_buffer_is_full(&it->tx_buffer)) {
      LL_USART_EnableIT_TXE_TXFNF(it->usart);
      continue;
    }
    uint8_ring_buffer_write(&it->tx_buffer, data[i]);
  }
  LL_USART_EnableIT_TXE_TXFNF(it->usart);
}

size_t usart_it_rx_peek(usart_it* it, uint32_t offset, uint8_t* buffer, size_t read_len) {
  return uint8_ring_buffer_peek(&it->rx_buffer, offset, buffer, read_len);
}

void usart_it_rx_skip(usart_it* it, size_t skip) {
  uint8_ring_buffer_skip(&it->rx_buffer, skip);
}

void usart_irq(usart_it* it) {
  if (LL_USART_IsActiveFlag_TXE_TXFNF(it->usart)) {
    if (uint8_ring_buffer_length(&it->tx_buffer) == 0) {
      LL_USART_DisableIT_TXE_TXFNF(it->usart);
    } else {
      uint8_t b = uint8_ring_buffer_read(&it->tx_buffer);
      // TXE flag will be automatically cleared when writing new data in TDR register
      LL_USART_TransmitData8(it->usart, b);
    }
  }

  if (LL_USART_IsActiveFlag_TC(it->usart)) {
    LL_USART_ClearFlag_TC(it->usart);
  }

  if (LL_USART_IsActiveFlag_RXNE_RXFNE(it->usart)) {
    // RXNE flag is cleared by reading of RDR register
    uint8_t b = LL_USART_ReceiveData8(it->usart);
    if (uint8_ring_buffer_is_full(&it->rx_buffer)) {
      Error_Handler();
    }
    uint8_ring_buffer_write(&it->rx_buffer, b);
  }

  if (LL_USART_IsActiveFlag_NE(it->usart)) {
    while (1)
      ;
  }
}