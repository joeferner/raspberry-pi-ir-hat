#include "dma_tx_ring_buffer.h"

static void dma_tx_ring_buffer_begin_dma_transfer(dma_tx_ring_buffer *rb);

static bool dma_tx_ring_buffer_is_full(dma_tx_ring_buffer *rb);

void dma_tx_ring_buffer_init(
    dma_tx_ring_buffer *rb,
    uint8_t *buffer,
    size_t buffer_length,
    DMA_TypeDef *dma,
    uint32_t dma_channel,
    dma_tx_begin_dma begin_dma
) {
    rb->buffer = buffer;
    rb->buffer_length = buffer_length;
    rb->dma = dma;
    rb->dma_channel = dma_channel;
    rb->tx_length = 0;
    rb->begin_dma = begin_dma;
    rb->read_offset = 0;
    rb->write_offset = 0;
    rb->begin_dma_lock = false;
}

void dma_tx_ring_buffer_write(dma_tx_ring_buffer *rb, const uint8_t *data, size_t data_len) {
    if (data_len > rb->buffer_length) {
        Error_Handler();
    }
    while (data_len > 0) {
        if (dma_tx_ring_buffer_is_full(rb)) {
            dma_tx_ring_buffer_begin_dma_transfer(rb);
        } else {
            rb->buffer[rb->write_offset] = *data++;
            rb->write_offset = (rb->write_offset + 1) % rb->buffer_length;
            data_len--;
        }
    }
    dma_tx_ring_buffer_begin_dma_transfer(rb);
}

static bool dma_tx_ring_buffer_is_full(dma_tx_ring_buffer *rb) {
    return ((rb->write_offset + 1) % rb->buffer_length) == rb->read_offset;
}

void dma_tx_ring_buffer_irq(dma_tx_ring_buffer *rb) {
    if (LL_DMA_GetDataLength(rb->dma, rb->dma_channel) == 0) {
        rb->read_offset = (rb->read_offset + rb->tx_length) % rb->buffer_length;
        rb->tx_length = 0;
        dma_tx_ring_buffer_begin_dma_transfer(rb);
    }
}

static void dma_tx_ring_buffer_begin_dma_transfer(dma_tx_ring_buffer *rb) {
    if (!rb->begin_dma_lock) {
        rb->begin_dma_lock = true;
        if (LL_DMA_GetDataLength(rb->dma, rb->dma_channel) == 0) {
            if (rb->read_offset <= rb->write_offset) {
                rb->tx_length = rb->write_offset - rb->read_offset;
            } else {
                rb->tx_length = rb->buffer_length - rb->read_offset;
            }
            if (rb->tx_length > 0) {
                volatile uint8_t *p = &rb->buffer[rb->read_offset];
                LL_DMA_DisableChannel(rb->dma, rb->dma_channel);
                LL_DMA_SetDataLength(rb->dma, rb->dma_channel, rb->tx_length);
                LL_DMA_SetMemoryAddress(rb->dma, rb->dma_channel, (uint32_t) p);
                LL_DMA_EnableChannel(rb->dma, rb->dma_channel);
                rb->begin_dma(rb);
            }
        }
        rb->begin_dma_lock = false;
    }
}
