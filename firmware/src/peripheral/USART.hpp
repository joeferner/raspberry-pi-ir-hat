#ifndef _PERIPHERAL_USART_HPP_
#define _PERIPHERAL_USART_HPP_

#include "hal/USART.hpp"

// TODO
// static uint8_t usart_tx_buffer[TX_BUFFER_LEN];
// static uint8_t usart_rx_buffer[RX_BUFFER_LEN];

namespace peripheral {
class USART {
 private:
  hal::USART* usart;

 public:
  USART(hal::USART* usart) : usart(usart) {
  }
  const void setup() const;
  const void loop() const;
};
}  // namespace peripheral

#endif
