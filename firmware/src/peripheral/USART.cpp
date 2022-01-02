#include "USART.hpp"

namespace peripheral {
const void USART::setup() const {
  // TODO
  // usart_it_init(&usart, DEBUG_USART, usart_tx_buffer, sizeof(usart_tx_buffer), usart_rx_buffer,
  //     sizeof(usart_rx_buffer));

  // void usart_it_init(usart_it * it, USART_TypeDef * usart, uint8_t * tx_buffer, size_t tx_buffer_length, uint8_t *
  // rx_buffer,
  //     size_t rx_buffer_length) {
  //     it->usart = usart;
  //     uint8_ring_buffer_init(&it->tx_buffer, tx_buffer, tx_buffer_length);
  //     uint8_ring_buffer_init(&it->rx_buffer, rx_buffer, rx_buffer_length);

  //     LL_USART_EnableIT_RXNE_RXFNE(it->usart);
  //     LL_USART_EnableIT_ERROR(it->usart);
  // }
}

const void USART::loop() const {
  // uint8_t peek_buffer[RX_BUFFER_LEN + 1];
  // size_t peek_size;
  // peek_size = usart_it_rx_peek(&usart, 0, peek_buffer, sizeof(peek_buffer));
  // for (size_t i = 0; i < peek_size; i++) {
  //     if (peek_buffer[i] == '\r' || peek_buffer[i] == '\n') {
  //         peek_buffer[i] = '\0';
  //         usart_it_rx_skip(&usart, i + 1);
  //         debug_rx((char*)peek_buffer);
  //         break;
  //     }
  // }
}
}  // namespace peripheral