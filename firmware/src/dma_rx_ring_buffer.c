#include "dma_rx_ring_buffer.h"

static size_t _dma_ring_buffer_get_pos(dma_rx_ring_buffer *rb);

void dma_rx_ring_buffer_init(
    dma_rx_ring_buffer *rb,
    uint8_t *buffer,
    size_t buffer_length,
    DMA_TypeDef *dma,
    uint32_t dma_channel
) {
    rb->buffer = buffer;
    rb->buffer_length = buffer_length;
    rb->dma = dma;
    rb->dma_channel = dma_channel;
    rb->read_offset = 0;
}

static size_t _dma_ring_buffer_get_pos(dma_rx_ring_buffer *rb) {
    return rb->buffer_length - LL_DMA_GetDataLength(rb->dma, rb->dma_channel);
}

size_t dma_rx_ring_buffer_peek(dma_rx_ring_buffer *rb, uint32_t offset, uint8_t *buffer, size_t read_len) {
    size_t write_offset = _dma_ring_buffer_get_pos(rb);
    size_t pos = rb->read_offset;

    // skip offset
    while (offset > 0 && pos != write_offset) {
        pos = (pos + 1) % rb->buffer_length;
        offset--;
    }

    size_t bytes_read = 0;
    while (read_len > 0 && pos != write_offset) {
        *buffer++ = rb->buffer[pos];
        pos = (pos + 1) % rb->buffer_length;
        bytes_read++;
    }
    return bytes_read;
}

void dma_rx_ring_buffer_skip(dma_rx_ring_buffer *rb, size_t len) {
    size_t write_offset = _dma_ring_buffer_get_pos(rb);

    // empty
    if (write_offset == rb->read_offset) {
        return;
    }

    // writing has wrapped
    if (write_offset < rb->read_offset) {
        size_t to_end_len = rb->buffer_length - rb->read_offset;
        if (to_end_len > len) {
            // skipping less than to end off buffer
            rb->read_offset += len;
            return;
        }
        rb->read_offset = 0;
        len -= to_end_len;
    }

    if (write_offset < len) {
        // asked to skip more than we have so just empty the ring
        rb->read_offset = write_offset;
        return;
    }

    rb->read_offset += len;
}
