#ifndef RPI_H
#define RPI_H

#include <stddef.h>
#include <stdint.h>

#define RPI_PACKET_RESPONSE  0x01
#define RPI_PACKET_TX_IR     0x02
#define RPI_PACKET_RX_IR     0x03

#define RPI_PACKET_IS_VALID_TYPE(type) ((type) == RPI_PACKET_TX_IR || (type) == RPI_PACKET_RESPONSE || (type) == RPI_PACKET_RX_IR)

typedef struct __attribute__((__packed__)) {
    uint8_t begin;
    uint8_t type;
    uint16_t size; /*!< unencoded packet size in bytes including header and end byte */
} rpi_packet_header;

typedef struct __attribute__((__packed__)) {
    rpi_packet_header header;
    uint32_t frequency;
    uint16_t signal_length; // length in number of elements not in bytes of the signal
    // uint32_t signal[];
} rpi_packet_tx_ir;

typedef struct __attribute__((__packed__)) {
    rpi_packet_header header;
    uint32_t value;
} rpi_packet_rx_ir;

void rpi_setup();

void rpi_loop();

void rpi_send_string(const char *str);

void rpi_send_uint32(uint32_t value);

void rpi_ir_rx_received(uint32_t value);

extern void rpi_rx(const rpi_packet_header *packet);

void rpi_dma_irq();

#endif
