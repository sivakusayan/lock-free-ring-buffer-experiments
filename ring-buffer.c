#include "ring-buffer.h" 

struct ring_buffer_header {
    size_t count;
    size_t item_size;
    size_t write_position;
    size_t read_position;
};

struct ring_buffer {
    struct ring_buffer_header header;
    char data[];
};

size_t ring_buffer_size(size_t count, size_t item_size) {
    return sizeof(struct ring_buffer_header) + count * item_size;
};
