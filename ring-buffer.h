#ifndef _RING_BUFFER_HEADER_GUARD
#define _RING_BUFFER_HEADER_GUARD

#include <stdlib.h>

// Return Codes
#define RING_BUFFER_OK			 0
#define RING_BUFFER_EMPTY		 -1
#define RING_BUFFER_FULL		 -2
#define RING_BUFFER_INVALID_ARGS -3

/**
 * Returns the size, in bytes, of the desired ring buffer.
 * Returns 0 if the computed size overflows, or if either
 * `capacity` or `item_size` are zero.
 */
size_t ring_buffer_size(size_t capacity, size_t item_size);

/**
 * Returns RING_BUFFER_INVALID_ARGS if either:
 * - `ring_buffer` is null
 * - `capacity` or `item_size` is zero
 *
 * Else returns RING_BUFFER_OK, and initializes a ring buffer into the
 * provided memory.
 *
 * The caller should use `ring_buffer_size()` to determine how large the
 * allocated memory being pointed to should be.
 */
int ring_buffer_init(void* ring_buffer, size_t capacity, size_t item_size);

/**
 * Returns RING_BUFFER_INVALID_ARGS if either `ring_buffer` or `item`
 * are null pointers.
 * Returns RING_BUFFER_FULL if the ring buffer has no more room.
 * Else, returns RING_BUFFER_OK and appends a copy of the data pointed
 * to by `item` to the start of the ring buffer.
 **/
int ring_buffer_push(void* ring_buffer, void* item);

/**
 * Returns RING_BUFFER_INVALID_ARGS if either `ring_buffer` or `item`
 * are null pointers.
 * Returns RING_BUFFER_EMPTY if the ring buffer has no items to pop.
 * Else, returns RING_BUFFER_OK and copies the member at the end of
 * the ring buffer into the `item` pointer, then removes that member.
 */
int ring_buffer_pop(void* ring_buffer, void* item);

#endif
