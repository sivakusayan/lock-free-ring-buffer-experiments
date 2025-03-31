#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "ring-buffer.h"

struct ring_header
{
	size_t item_size;
	void* write_ptr;
	void* read_ptr;
	void* buffer_end_ptr;
};

struct ring_buffer
{
	struct ring_header header;
	void* buffer;
};

static struct ring_header* get_header(void* ring_buffer)
{
	return &((struct ring_buffer*)ring_buffer)->header;
}

static void* get_buffer(void* ring_buffer)
{
	return &((struct ring_buffer*)ring_buffer)->buffer;
}

size_t ring_buffer_size(size_t capacity, size_t item_size)
{
	if (capacity == 0 || item_size == 0) { return 0; }
	capacity += 1;

	// Make sure that the buffer size will not overflow.
	if (capacity > SIZE_MAX / item_size) { return 0; }
	size_t buffer_size = capacity * item_size;

	// Even if the buffer size didn't overflow, make sure adding the header size
	// to it won't either!
	if (buffer_size > SIZE_MAX - sizeof(struct ring_header)) { return 0; }
	return sizeof(struct ring_header) + buffer_size;
};

int ring_buffer_init(void* ring_buffer, size_t capacity, size_t item_size)
{
	if (!ring_buffer) { return RING_BUFFER_INVALID_ARGS; }
	if (capacity == 0 || item_size == 0) { return RING_BUFFER_INVALID_ARGS; }

	struct ring_header* header = get_header(ring_buffer);
	header->item_size = item_size;

	void* start = ring_buffer + sizeof(struct ring_header);
	header->write_ptr = start;
	header->read_ptr = start;
	header->buffer_end_ptr = start + item_size * (capacity + 1);
	return RING_BUFFER_OK;
}

/**
 * Gives the next index in our ring buffer given some starting index,
 * wrapping around the ring buffer if necessary.
 */
static void* get_next_ptr(void* start_ptr,
						  void* current_ptr,
						  void* end_ptr,
						  size_t item_size)
{
	assert(current_ptr <= end_ptr);
	if (current_ptr + item_size >= end_ptr) { return start_ptr; }
	return current_ptr + item_size;
}

int ring_buffer_push(void* restrict ring_buffer, void* restrict item)
{
	assert(ring_buffer);
	assert(item);
	struct ring_header* hdr = get_header(ring_buffer);
	void* buffer = get_buffer(ring_buffer);

	void* next_write_ptr = get_next_ptr(
		buffer, hdr->write_ptr, hdr->buffer_end_ptr, hdr->item_size);
	if (next_write_ptr == hdr->read_ptr) { return RING_BUFFER_FULL; }

	memcpy(hdr->write_ptr, item, hdr->item_size);
	hdr->write_ptr = next_write_ptr;

	return RING_BUFFER_OK;
};

int ring_buffer_pop(void* restrict ring_buffer, void* restrict item)
{
	assert(ring_buffer);
	assert(item);
	struct ring_header* hdr = get_header(ring_buffer);
	void* buffer = get_buffer(ring_buffer);

	if (hdr->read_ptr == hdr->write_ptr) { return RING_BUFFER_EMPTY; }

	memcpy(item, hdr->read_ptr, hdr->item_size);
	hdr->read_ptr = get_next_ptr(
		buffer, hdr->read_ptr, hdr->buffer_end_ptr, hdr->item_size);

	return RING_BUFFER_OK;
};
