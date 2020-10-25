
#include "config.h"
#include "uart.h"
#include "ir_rx.h"

void setup() {
    uart_setup();
    ir_rx_setup();
    uart_send_string("READY\n");
}

void loop() {
    uart_loop();
    ir_rx_loop();
}

void main_error_handler() {
    while (1);
}

void uart_rx(const uint8_t *data, size_t data_len) {
    uart_send_string("OK\n");
    uart_tx(data, data_len);
    uart_send_string("\n");
}

void ir_rx_received(ir_rx_value value) {
    uart_send_string("P");
    uart_send_uint32(value);
    uart_send_string("\n");
}