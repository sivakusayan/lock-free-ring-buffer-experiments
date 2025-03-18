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
TEST(
	"Returns 0 when the size of the buffer together with the ring buffer "
	"header would overflow.")
{
	ASSERT(ring_buffer_size(1, SIZE_MAX - 1) == 0);
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

TESTGROUP("ring_buffer_push")
TEST("Can successfully push an element.")
{
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);

	int test = 3;
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);

	free(buffer);
}
TEST("Can push an element up to the capacity of the ring buffer")
{
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);

	int test = 3;
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);

	free(buffer);
}
TEST(
	"Pushing more than the capacity of the ring buffer returns "
	"RING_BUFFER_FULL")
{
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);

	int test = 3;
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_FULL);

	free(buffer);
}
ENDTESTGROUP()

TESTGROUP("ring_buffer_pop")
TEST("Can successfully pop an element.")
{
	int test = 3;
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &test) == RING_BUFFER_OK);

	int out;
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == test);

	free(buffer);
}
TEST("Can pop elements until there are no more elements left over.")
{
	int first = 3;
	int second = 4;
	int third = 5;
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &first) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &second) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &third) == RING_BUFFER_OK);

	int out;
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == first);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == second);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == third);

	free(buffer);
}
TEST("Popping more elements than there are available returns RING_BUFFER_EMPTY")
{
	int out;
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	int first = 3;
	int second = 4;
	int third = 5;
	ASSERT(ring_buffer_push(buffer, &first) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &second) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &third) == RING_BUFFER_OK);

	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == first);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == second);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == third);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	free(buffer);
}
ENDTESTGROUP()
#endif
