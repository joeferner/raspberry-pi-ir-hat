#include "main.h"

#include <memory.h>
#include <stdlib.h>

#include "current_sensor.hpp"
#include "hal/Bus.hpp"
#include "hal/Clocks.hpp"
#include "hal/DMA.hpp"
#include "hal/GPIO.hpp"
#include "hal/IWDG.hpp"
#include "hal/NVIC.hpp"
#include "hal/RCC.hpp"
#include "hal/System.hpp"
#include "hal/Timer.hpp"
#include "hal/USART.hpp"
#include "ir_rx.hpp"
#include "ir_tx.hpp"
#include "peripheral/USART.hpp"
#include "setup.hpp"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

hal::System halSystem;
hal::Clocks clocks;
hal::Bus bus;
hal::NVICHal nvic;
hal::RCCHal rcc;
hal::GPIO<hal::gpio::GPIOAddress::GPIOFAddress, hal::gpio::GPIOPin::Pin2> resetPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin7> irInLedPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin9> irOutPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin7> usart1RxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOBAddress, hal::gpio::GPIOPin::Pin6> usart1TxPin;
hal::USART<hal::usart::USARTAddress::USART1Address> usart1;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin3> usart2RxPin;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin2> usart2TxPin;
hal::USART<hal::usart::USARTAddress::USART2Address> usart2;
hal::GPIO<hal::gpio::GPIOAddress::GPIOAAddress, hal::gpio::GPIOPin::Pin6> irRxPin;
hal::DMAChannel<hal::dma::DMAAddress::DMA1Address, hal::dma::Channel::Channel5> irRxDmaChannel;
hal::Timer<hal::timer::TimerAddress::TIM3Address> irRxTimer;
hal::Timer<hal::timer::TimerAddress::TIM17Address> irTxCarrierTimer;
hal::Timer<hal::timer::TimerAddress::TIM16Address> irTxSignalTimer;
hal::IWDGHal iwdg;

peripheral::USART<hal::usart::USARTAddress::USART1Address, DEBUG_TX_BUFFER_SIZE, DEBUG_RX_BUFFER_SIZE> debugUsart(
    &usart1);
peripheral::USART<hal::usart::USARTAddress::USART2Address, DEBUG_TX_BUFFER_SIZE, DEBUG_RX_BUFFER_SIZE> rpiUsart(
    &usart2);

#define IR_TX_BUFFER_LEN_BEFORE_SEND 10

typedef void (*send_string)(const char*);

void setupSystemClock();

void loop();

void process_rx(char* data, send_string send_string);

int main() {
  setup();
  while (1) {
    loop();
  }
}

void loop() {
  char buffer[DEBUG_RX_BUFFER_SIZE];
  size_t lineLen;

  LL_IWDG_ReloadCounter(IWDG);
  if ((lineLen = debugUsart.readLine(buffer, sizeof(buffer)))) {
    debugUsart.write("OK\n");
  }
  if ((lineLen = rpiUsart.readLine(buffer, sizeof(buffer)))) {
    rpiUsart.write("OK\n");
  }
  ir_rx_loop();
  current_sensor_loop();
}

// TODO
// void debug_rx(char* data) {
//   process_rx(data, debug_send_string);
// }

// void rpi_rx(char* data) {
//   process_rx(data, rpi_send_string);
// }

// void process_rx(char* data, send_string send_string) {
//   if (strcmp(data, "+iwdg") == 0) {
//     send_string("+OK\n");
//     while (1)
//       ;
//   } else if (strcmp(data, "+send") == 0) {
//     ir_tx_send();
//     send_string("+OK\n");
//   } else if (strncmp(data, "+f", 2) == 0) {
//     uint32_t carrier_freq = atoi(data + 2);
//     ir_tx_reset(carrier_freq);
//     send_string("+OK\n");
//   } else if (strncmp(data, "+s", 2) == 0) {
//     char* pon = data + 2;
//     char* p = strchr(pon, ',');
//     if (p == NULL) {
//       send_string("-ERR missing comma\n");
//       return;
//     }
//     *p = '\0';
//     uint32_t t_on = atoi(pon);
//     uint32_t t_off = atoi(p + 1);
//     ir_tx_write(t_on, t_off);
//     if (ir_tx_buffer_length() > IR_TX_BUFFER_LEN_BEFORE_SEND) {
//       ir_tx_send();
//     }
//     send_string("+OK\n");
//   } else if (strncmp(data, "+c", 2) == 0) {
//     uint16_t d;
//     if (data[2] == '0') {
//       d = current_sensor_get0();
//     } else if (data[2] == '1') {
//       d = current_sensor_get1();
//     } else {
//       send_string("-ERR invalid channel\n");
//       return;
//     }
//     char buffer[50];
//     strcpy(buffer, "+OK ");
//     itoa(d, buffer + strlen(buffer), 10);
//     strcat(buffer, "\n");
//     send_string(buffer);
//   } else {
//     send_string("-ERR \"");
//     send_string(data);
//     send_string("\"\n");
//   }
// }

void ir_rx_received(uint32_t value) {
  char buffer[20];
  buffer[0] = '!';
  buffer[1] = 's';
  utoa(value, buffer + 2, 10);
  strcat(buffer, "\n");

  // TODO
  // rpi_send_string(buffer);
  // debug_send_string(buffer);
}

void current_sensor_overrun_error() {
  // TODO
  // const char* str = "-ERR current sensor overrun\n";
  // rpi_send_string(str);
  // debug_send_string(str);
}

void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}

#ifdef __cplusplus
}
#endif
