#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "ring-buffer.h"

struct ring_header
{
	size_t item_size;
	size_t write_index;
	size_t read_index;
	size_t max_index;
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

static size_t get_capacity_to_allocate(size_t capacity, size_t item_size)
{
	size_t allocated_capacity = 0x1;
	while (allocated_capacity - 1 < capacity) {
		if (allocated_capacity > SIZE_MAX / item_size) { return 0; }
		allocated_capacity = allocated_capacity << 1;
	}
	return allocated_capacity;
}

size_t ring_buffer_size(size_t capacity, size_t item_size)
{
	if (capacity == 0 || item_size == 0) { return 0; }

	size_t capacity_to_allocate = get_capacity_to_allocate(capacity, item_size);
	if (capacity_to_allocate == 0) { return 0; }
	size_t buffer_size = capacity_to_allocate * item_size;

	// Even if the buffer size didn't overflow, make sure adding the header size
	// to it won't either!
	if (buffer_size > SIZE_MAX - sizeof(struct ring_header)) { return 0; }
	return sizeof(struct ring_header) + buffer_size;
};

int ring_buffer_init(void* ring_buffer, size_t capacity, size_t item_size)
{
	if (!ring_buffer) { return RING_BUFFER_INVALID_ARGS; }
	if (capacity == 0 || item_size == 0) { return RING_BUFFER_INVALID_ARGS; }
	size_t capacity_to_allocate = get_capacity_to_allocate(capacity, item_size);
	if (capacity_to_allocate == 0) { return RING_BUFFER_INVALID_ARGS; }

	struct ring_header* header = get_header(ring_buffer);
	header->max_index = capacity_to_allocate - 1;
	header->item_size = item_size;
	header->write_index = 0;
	header->read_index = 0;
	return RING_BUFFER_OK;
}

/**
 * Gives the next index in our ring buffer given some starting index,
 * wrapping around the ring buffer if necessary.
 */
static size_t get_next_index(size_t current_index, size_t max_index)
{
	assert(current_index <= max_index);
	return (current_index + 1) & max_index;
}

int ring_buffer_push(void* restrict ring_buffer, void* restrict item)
{
	assert(ring_buffer);
	assert(item);
	struct ring_header* hdr = get_header(ring_buffer);

	size_t next_write_index = get_next_index(hdr->write_index, hdr->max_index);
	if (next_write_index == hdr->read_index) { return RING_BUFFER_FULL; }

	void* buffer = get_buffer(ring_buffer);
	memcpy(buffer + (hdr->write_index * hdr->item_size), item, hdr->item_size);
	hdr->write_index = next_write_index;

	return RING_BUFFER_OK;
};

int ring_buffer_pop(void* restrict ring_buffer, void* restrict item)
{
	assert(ring_buffer);
	assert(item);
	struct ring_header* hdr = get_header(ring_buffer);
	if (hdr->read_index == hdr->write_index) { return RING_BUFFER_EMPTY; }

	void* buffer = get_buffer(ring_buffer);
	memcpy(item, buffer + (hdr->read_index * hdr->item_size), hdr->item_size);
	hdr->read_index = get_next_index(hdr->read_index, hdr->max_index);

	return RING_BUFFER_OK;
};
