#ifndef _uint32_ring_buffer_h_
#define _uint32_ring_buffer_h_

#include <stddef.h>
#include <stdint.h>

typedef struct {
  volatile uint32_t* buffer;
  size_t buffer_count;
  size_t read;
  size_t write;
  size_t length;
} uint32_ring_buffer;

void uint32_ring_buffer_init(uint32_ring_buffer* rb, uint32_t* buffer, size_t buffer_count);

size_t uint32_ring_buffer_length(uint32_ring_buffer* rb);

void uint32_ring_buffer_write(uint32_ring_buffer* rb, uint32_t item);

uint32_t uint32_ring_buffer_read(uint32_ring_buffer* rb);

#endif
