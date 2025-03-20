#if defined HEADERS
#include <stdint.h>
#include "ring-buffer.h"
void* buffer;
struct test_struct
{
	double some_double;
	int some_int;
};
static void test_struct_assert_equal(struct test_struct a, struct test_struct b)
{
	ASSERT(a.some_double == b.some_double && a.some_int == b.some_int);
}

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
TEST("Returns 0 when either the size or count are 0.")
{
	ASSERT(ring_buffer_size(0, 1) == 0);
	ASSERT(ring_buffer_size(1, 0) == 0);
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

	int in = 3;
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);

	free(buffer);
}
TEST(
	"Can push an element up to the capacity of a ring buffer that can only "
	"hold one element.")
{
	buffer = malloc(ring_buffer_size(1, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 1, sizeof(int)) == RING_BUFFER_OK);

	int in = 3;
	int out;
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_FULL);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_FULL);

	free(buffer);
}
TEST("Can push an element up to the capacity of the ring buffer")
{
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);

	int in = 3;
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_FULL);

	free(buffer);
}
TEST(
	"Can push, wrapping around the ring buffer, up to the capacity of the ring "
	"buffer.")
{
	// First, set the write pointer to be somewhere in the middle of the buffer.
	int in = 3;
	int out;
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);

	// Next, make sure that we can have writes wrap around the buffer.
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_FULL);

	free(buffer);
}
TEST(
	"Can push structures, wrapping around the ring buffer, up to the capacity "
	"of the ring buffer.")
{
	// First, set the write pointer to be somewhere in the middle of the buffer.
	struct test_struct in = {.some_double = 3, .some_int = 2};
	struct test_struct out;
	buffer = malloc(ring_buffer_size(3, sizeof(struct test_struct)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);

	// Next, make sure that we can have writes wrap around the buffer.
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_FULL);

	free(buffer);
}
ENDTESTGROUP()

TESTGROUP("ring_buffer_pop")
TEST("Can successfully pop an element.")
{
	int in = 3;
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &in) == RING_BUFFER_OK);

	int out;
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == in);

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
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	free(buffer);
}
TEST(
	"Can pop elements until there are no elements left over, even when "
	"wrapping around the ring buffer.")
{
	int out;
	buffer = malloc(ring_buffer_size(3, sizeof(int)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(int)) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	// First: Move the read pointer to the middle of the buffer.
	int first = 3;
	int second = 4;
	int third = 5;
	ASSERT(ring_buffer_push(buffer, &first) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &second) == RING_BUFFER_OK);

	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == first);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	ASSERT(out == second);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	// Second: Add some more elements, then test that wrap-around
	// behavior works when popping.
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
TEST(
	"Can pop structures until there are no elements left over, even when "
	"wrapping around the ring buffer.")
{
	struct test_struct out;
	buffer = malloc(ring_buffer_size(3, sizeof(struct test_struct)));
	ASSERT(ring_buffer_init(buffer, 3, sizeof(struct test_struct)) ==
		   RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	// First: Move the read pointer to the middle of the buffer.
	struct test_struct first = {.some_double = 0, .some_int = 1};
	struct test_struct second = {.some_double = 2, .some_int = 3};
	struct test_struct third = {.some_double = 4, .some_int = 5};
	ASSERT(ring_buffer_push(buffer, &first) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &second) == RING_BUFFER_OK);

	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	test_struct_assert_equal(out, first);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	test_struct_assert_equal(out, second);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	// Second: Add some more elements, then test that wrap-around
	// behavior works when popping.
	ASSERT(ring_buffer_push(buffer, &first) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &second) == RING_BUFFER_OK);
	ASSERT(ring_buffer_push(buffer, &third) == RING_BUFFER_OK);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	test_struct_assert_equal(out, first);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	test_struct_assert_equal(out, second);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_OK);
	test_struct_assert_equal(out, third);
	ASSERT(ring_buffer_pop(buffer, &out) == RING_BUFFER_EMPTY);

	free(buffer);
}

ENDTESTGROUP()
#endif
