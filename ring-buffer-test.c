#if defined HEADERS
#include <stdint.h>
#include "ring-buffer.h"
void* buffer;

#elif defined TESTS
TESTGROUP("ring_buffer_size")
TEST("Returns 0 when the size of the underlying buffer would overflow.")
{
    ASSERT(ring_buffer_size(2, SIZE_MAX) == 0);
}
TEST("Returns 0 when the size of the buffer together with the ring buffer header "
     "would overflow.")
{
    ASSERT(ring_buffer_size(1, SIZE_MAX-1) == 0);
}
TEST("Returns a non-zero size if a valid ring buffer size can be calculated.")
{
    ASSERT(ring_buffer_size(10, 10) > 0);
}
ENDTESTGROUP()

TESTGROUP("ring_buffer_init")
TEST("Returns RING_BUFFER_INVALID_ARGS when passed a null buffer.")
{
    ASSERT(ring_buffer_init(NULL, 1, 1) == RING_BUFFER_INVALID_ARGS);
}
TEST("Returns RING_BUFFER_INVALID_ARGS when count or item_size is zero")
{
    buffer = malloc(ring_buffer_size(1, 1));
    ASSERT(ring_buffer_init(buffer, 0, 1) == RING_BUFFER_INVALID_ARGS);
    ASSERT(ring_buffer_init(buffer, 1, 0) == RING_BUFFER_INVALID_ARGS);
    ASSERT(ring_buffer_init(buffer, 0, 0) == RING_BUFFER_INVALID_ARGS);
    free(buffer);
}
TEST("Returns RING_BUFFER_OK if a valid ring buffer can be constructed.")
{
    buffer = malloc(ring_buffer_size(1, 1));
    ASSERT(ring_buffer_init(buffer, 1, 1) == RING_BUFFER_OK);
    free(buffer);

    buffer = malloc(ring_buffer_size(1000, sizeof(int)));
    ASSERT(ring_buffer_init(buffer, 1000, sizeof(int)) == RING_BUFFER_OK);
    free(buffer);
}
ENDTESTGROUP()
#endif
