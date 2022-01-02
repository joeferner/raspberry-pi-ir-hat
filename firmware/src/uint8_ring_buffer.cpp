#include "uint8_ring_buffer.hpp"

#include "main.h"

#define GUARD_CHAR '?'

void uint8_ring_buffer_init(uint8_ring_buffer* rb, uint8_t* buffer, size_t buffer_count) {
  rb->buffer = buffer;
  rb->buffer_count = buffer_count;
  rb->read = 0;
  rb->write = 0;
  rb->length = 0;
  for (size_t i = 0; i < buffer_count; i++) {
    rb->buffer[i] = GUARD_CHAR;
  }
}

size_t uint8_ring_buffer_length(uint8_ring_buffer* rb) {
  return rb->length;
}

bool uint8_ring_buffer_is_empty(uint8_ring_buffer* rb) {
  return rb->length == 0;
}

bool uint8_ring_buffer_is_full(uint8_ring_buffer* rb) {
  return rb->length == rb->buffer_count;
}

void uint8_ring_buffer_write(uint8_ring_buffer* rb, uint8_t item) {
  DISABLE_IRQS();
  if (uint8_ring_buffer_is_full(rb)) {
    Error_Handler();
    ENABLE_IRQS();
    return;
  }
  if (rb->buffer[rb->write] != GUARD_CHAR) {
    while (1)
      ;
  }
  rb->buffer[rb->write] = item;
  rb->write = (rb->write + 1) % rb->buffer_count;
  rb->length++;
  ENABLE_IRQS();
}

uint8_t uint8_ring_buffer_read(uint8_ring_buffer* rb) {
  DISABLE_IRQS();
  if (uint8_ring_buffer_is_empty(rb)) {
    Error_Handler();
    ENABLE_IRQS();
    return 0;
  }
  uint8_t result = rb->buffer[rb->read];
  rb->buffer[rb->read] = GUARD_CHAR;
  rb->read = (rb->read + 1) % rb->buffer_count;
  rb->length--;
  ENABLE_IRQS();
  return result;
}

size_t uint8_ring_buffer_peek(uint8_ring_buffer* rb, uint32_t offset, uint8_t* buffer, size_t read_len) {
  DISABLE_IRQS();
  size_t write = rb->write;
  size_t read = rb->read;

  // skip offset
  while (offset > 0 && read != write) {
    read = (read + 1) % rb->buffer_count;
    offset--;
  }

  size_t bytes_read = 0;
  while (read_len > 0 && read != write) {
    *buffer++ = rb->buffer[read];
    read = (read + 1) % rb->buffer_count;
    bytes_read++;
    read_len--;
  }

  ENABLE_IRQS();
  return bytes_read;
}

void uint8_ring_buffer_skip(uint8_ring_buffer* rb, size_t skip) {
  DISABLE_IRQS();
  while (skip > 0 && rb->read != rb->write) {
    rb->buffer[rb->read] = GUARD_CHAR;
    rb->read = (rb->read + 1) % rb->buffer_count;
    rb->length--;
    skip--;
  }
  ENABLE_IRQS();
}
