#if defined HEADERS
#include <stdint.h>
#include "ring-buffer.h"
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
TEST("Returns a non-zero size if a valid ring buffer can be constructed.")
{
    ASSERT(ring_buffer_size(10, 10) > 0);
}
ENDTESTGROUP()
#endif
