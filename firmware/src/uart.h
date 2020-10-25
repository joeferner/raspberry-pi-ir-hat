#ifndef _uart_h_
#define _uart_h_

#include <string.h>
#include <stdint.h>

void uart_setup();

void uart_send_string(const char *str);

void uart_tx(const uint8_t *data, size_t data_len);

#endif