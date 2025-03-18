#include <assert.h>
#include <stdint.h>
#include "ring-buffer.h"

struct ring_buffer_header
{
	size_t count;
	size_t item_size;
	size_t write_index;
	size_t read_index;
};

struct ring_buffer
{
	struct ring_buffer_header header;
	char buffer[];
};

size_t ring_buffer_size(size_t count, size_t item_size)
{
	assert(count > 0);
	assert(item_size > 0);

	// Make sure that the buffer size will not overflow.
	if (count > SIZE_MAX / item_size) { return 0; }
	size_t buffer_size = count * item_size;

	// Even if the buffer size didn't overflow, make sure adding the header size
	// to it won't either!
	if (buffer_size > SIZE_MAX - sizeof(struct ring_buffer_header)) {
		return 0;
	}
	return sizeof(struct ring_buffer_header) + buffer_size;
};

int ring_buffer_init(void* ring_buffer, size_t count, size_t item_size)
{
	if (!ring_buffer) { return RING_BUFFER_INVALID_ARGS; }
	if (count == 0 || item_size == 0) { return RING_BUFFER_INVALID_ARGS; }

	struct ring_buffer_header header =
		((struct ring_buffer*)ring_buffer)->header;
	header.count = count;
	header.item_size = item_size;
	header.write_index = 0;
	header.read_index = 0;
	return RING_BUFFER_OK;
}
