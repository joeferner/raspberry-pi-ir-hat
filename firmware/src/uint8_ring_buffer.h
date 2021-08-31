#ifndef _uint8_ring_buffer_h_
#define _uint8_ring_buffer_h_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  volatile uint8_t* buffer;
  size_t buffer_count;
  volatile size_t read;
  volatile size_t write;
  volatile size_t length;
} uint8_ring_buffer;

void uint8_ring_buffer_init(uint8_ring_buffer* rb, uint8_t* buffer, size_t buffer_count);

size_t uint8_ring_buffer_length(uint8_ring_buffer* rb);

bool uint8_ring_buffer_is_empty(uint8_ring_buffer* rb);

bool uint8_ring_buffer_is_full(uint8_ring_buffer* rb);

void uint8_ring_buffer_write(uint8_ring_buffer* rb, uint8_t item);

size_t uint8_ring_buffer_peek(uint8_ring_buffer* rb, uint32_t offset, uint8_t* buffer, size_t read_len);

void uint8_ring_buffer_skip(uint8_ring_buffer* rb, size_t skip);

uint8_t uint8_ring_buffer_read(uint8_ring_buffer* rb);

#endif
