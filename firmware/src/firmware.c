
#include "uart.h"

void setup() {
    uart_setup();
    uart_send_string("READY\n");
}

void loop() {
    uart_loop();
}

void main_error_handler() {
    while(1);
}

void uart_rx(const uint8_t *data, size_t data_len) {
    uart_send_string("OK\n");
    uart_tx(data, data_len);
    uart_send_string("\n");
}
