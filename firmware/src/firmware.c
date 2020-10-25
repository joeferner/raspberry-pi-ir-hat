
#include "uart.h"

void setup() {
    uart_setup();
    uart_send_string("Hello");
}

void loop() {
    uart_send_string("Hello");
}

void main_error_handler() {
    while(1);
}
