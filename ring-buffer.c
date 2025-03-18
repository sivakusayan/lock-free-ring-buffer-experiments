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

size_t ring_buffer_size(size_t count, size_t item_size)
{
	assert(count > 0);
	assert(item_size > 0);

	// Make sure that the buffer size will not overflow.
	if (count + 1 > SIZE_MAX / item_size) { return 0; }
	size_t buffer_size = (count + 1) * item_size;

	// Even if the buffer size didn't overflow, make sure adding the header size
	// to it won't either!
	if (buffer_size > SIZE_MAX - sizeof(struct ring_header)) { return 0; }
	return sizeof(struct ring_header) + buffer_size;
};

int ring_buffer_init(void* ring_buffer, size_t count, size_t item_size)
{
	if (!ring_buffer) { return RING_BUFFER_INVALID_ARGS; }
	if (count == 0 || item_size == 0) { return RING_BUFFER_INVALID_ARGS; }

	struct ring_header* header = get_header(ring_buffer);
	header->max_index = count;
	header->item_size = item_size;
	header->write_index = 0;
	header->read_index = 0;
	return RING_BUFFER_OK;
}

static size_t get_next_index(size_t current_index, size_t max_index)
{
	assert(current_index <= max_index);
	if (current_index + 1 > max_index) { return 0; }
	return current_index + 1;
}

int ring_buffer_push(void* ring_buffer, void* item)
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

int ring_buffer_pop(void* ring_buffer, void* item)
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
