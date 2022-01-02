#include "uint32_ring_buffer.hpp"
#include "main.h"

void uint32_ring_buffer_init(uint32_ring_buffer* rb, uint32_t* buffer, size_t buffer_count) {
  rb->buffer = buffer;
  rb->buffer_count = buffer_count;
  rb->read = 0;
  rb->write = 0;
  rb->length = 0;
}

size_t uint32_ring_buffer_length(uint32_ring_buffer* rb) {
  return rb->length;
}

void uint32_ring_buffer_write(uint32_ring_buffer* rb, uint32_t item) {
  DISABLE_IRQS();
  rb->buffer[rb->write] = item;
  rb->write = (rb->write + 1) % rb->buffer_count;
  rb->length++;
  ENABLE_IRQS();
}

uint32_t uint32_ring_buffer_read(uint32_ring_buffer* rb) {
  DISABLE_IRQS();
  uint32_t result = rb->buffer[rb->read];
  rb->read = (rb->read + 1) % rb->buffer_count;
  rb->length--;
  ENABLE_IRQS();
  return result;
}
