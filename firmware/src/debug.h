#ifndef DEBUG_H
#define DEBUG_H

#include <stddef.h>
#include <stdint.h>

void debug_setup();

void debug_loop();

void debug_send_string(const char *str);

void debug_send_uint32(uint32_t value);

void debug_tx(const uint8_t *data, size_t data_len);

extern void debug_rx( char *data);

void debug_usart_irq();

#endif
