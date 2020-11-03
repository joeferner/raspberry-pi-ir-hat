#ifndef _debug_h_
#define _debug_h_

#include <stddef.h>
#include <stdint.h>

void debug_setup();

void debug_loop();

void debug_send_string(const char *str);

void debug_send_uint32(uint32_t value);

void debug_tx(const uint8_t *data, size_t data_len);

extern void debug_rx(const uint8_t *data, size_t data_len);

void debug_dma_irq();

#endif
