#ifndef _RING_BUFFER_HEADER_GUARD
#define _RING_BUFFER_HEADER_GUARD

#include <stdlib.h>

// Return Codes
#define RING_BUFFER_OK 0
#define RING_BUFFER_EMPTY -1
#define RING_BUFFER_FULL -2

/**
 * Returns the size, in bytes, of the desired ring buffer.
 * Returns 0 if the computed size overflows. 
 */
size_t ring_buffer_size(size_t count, size_t item_size);
/**
 * Initializes a ring buffer into the provided buffer. The caller should use
 * ring_buffer_size() to determine how big the buffer should be.
 */
void ring_buffer_init(void* ring_buffer, size_t count, size_t item_size);
/**
 * Returns RING_BUFFER_FULL if the ring buffer has no more room, else
 * returns RING_BUFFER_OK.
 */
int ring_buffer_push(void* ring_buffer, void* item);
/**
 * Returns RING_BUFFER_EMPTY if the ring buffer has no items to pop,
 * else returns RING_BUFFER_OK.
 */
int ring_buffer_pop(void* ring_buffer, void* item);

#endif
