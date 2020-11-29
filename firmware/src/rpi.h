#ifndef RPI_H
#define RPI_H

#include <stddef.h>
#include <stdint.h>

void rpi_setup();

void rpi_loop();

void rpi_send_string(const char *str);

void rpi_send_uint32(uint32_t value);

void rpi_tx(const uint8_t *data, size_t data_len);

extern void rpi_rx(const uint8_t *data, size_t data_len);

void rpi_dma_irq();

#endif
