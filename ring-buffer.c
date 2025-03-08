#include "ring-buffer.h"

struct ring_buffer_header {
    size_t count;
    size_t item_size;
    size_t write_position;
    size_t read_position;
};

struct ring_buffer {
    struct ring_buffer_header header;
    char buffer[];
};

size_t ring_buffer_size(size_t count, size_t item_size) {
    if (count > SIZE_MAX / item_size) return 0;
    size_t buffer_size = count * item_size;
    if (buffer_size > SIZE_MAX - sizeof(struct ring_buffer_header)) return 0;
    return sizeof(struct ring_buffer_header) + buffer_size;
};
