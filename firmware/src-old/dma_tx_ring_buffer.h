#ifndef DMA_TX_RING_BUFFER_H
#define DMA_TX_RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "config.h"

struct dma_tx_ring_buffer_t;

typedef void (*dma_tx_begin_dma)(struct dma_tx_ring_buffer_t *rb);

typedef struct dma_tx_ring_buffer_t {
  volatile uint8_t *buffer;
  size_t buffer_length;
  DMA_TypeDef *dma;
  uint32_t dma_channel;
  volatile size_t
      write_offset; /*!< Next write offset. Buffer is considered empty when `r == w` and full when `w == r - 1` */
  volatile size_t
      read_offset; /*!< Next read offset. Buffer is considered empty when `r == w` and full when `w == r - 1` */
  volatile size_t tx_length; /*!< length of data currently transferring via DMA request */
  volatile bool begin_dma_lock;
  dma_tx_begin_dma begin_dma;
} dma_tx_ring_buffer;

void dma_tx_ring_buffer_init(dma_tx_ring_buffer *rb, uint8_t *buffer, size_t buffer_length, DMA_TypeDef *dma,
                             uint32_t dma_channel, dma_tx_begin_dma begin_dma);

void dma_tx_ring_buffer_irq(dma_tx_ring_buffer *rb);

void dma_tx_ring_buffer_write(dma_tx_ring_buffer *rb, const uint8_t *data, size_t data_len);

#endif
