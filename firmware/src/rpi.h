#ifndef RPI_H
#define RPI_H

#include <stddef.h>
#include <stdint.h>

#define RPI_PACKET_RESPONSE  0x01
#define RPI_PACKET_IR_SIGNAL 0x02

#define RPI_PACKET_IS_VALID_TYPE(type) ((type) == RPI_PACKET_IR_SIGNAL || (type) == RPI_PACKET_RESPONSE)

typedef struct __attribute__((__packed__)) {
    uint8_t begin;
    uint8_t type;
    uint16_t size; /*!< unencoded packet size in bytes including header */
} rpi_packet_header;

typedef struct __attribute__((__packed__)) {
    rpi_packet_header header;
} rpi_packet_ir_signal;

void rpi_setup();

void rpi_loop();

void rpi_send_string(const char *str);

void rpi_send_uint32(uint32_t value);

void rpi_tx(const uint8_t *data, size_t data_len);

extern void rpi_rx(const rpi_packet_header *packet);

void rpi_dma_irq();

#endif
