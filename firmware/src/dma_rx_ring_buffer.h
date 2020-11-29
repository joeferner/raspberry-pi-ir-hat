#ifndef DMA_RX_RING_BUFFER_H
#define DMA_RX_RING_BUFFER_H

#include "config.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    volatile uint8_t *buffer;
    size_t buffer_length;
    DMA_TypeDef *dma;
    uint32_t dma_channel;
    volatile size_t read_offset; /*!< Next read offset. Buffer is considered empty when `r == w` and full when `w == r - 1` */
} dma_rx_ring_buffer;

void dma_rx_ring_buffer_init(
    dma_rx_ring_buffer *rb,
    uint8_t *buffer,
    size_t buffer_length,
    DMA_TypeDef *dma,
    uint32_t dma_channel
);

size_t dma_rx_ring_buffer_peek(dma_rx_ring_buffer *rb, uint32_t offset, uint8_t *buffer, size_t read_len);

void dma_rx_ring_buffer_skip(dma_rx_ring_buffer *rb, size_t len);

#endif
