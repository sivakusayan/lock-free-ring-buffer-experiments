#include <stdio.h>
#include <stdlib.h>
#include "ring-buffer.h"
#define CAPACITY (100 * 1000 * 1000)

struct test_struct
{
	double some_double;
	int some_int;
};

int main(void)
{
	struct test_struct in = {.some_double = 10, .some_int = 5};
	size_t size = ring_buffer_size(CAPACITY, sizeof(struct test_struct));
	if (size == 0) {
		puts("Failed to initialize ring buffer.");
		exit(EXIT_FAILURE);
	}
	void* buffer = malloc(size);
	int result = ring_buffer_init(buffer, CAPACITY, sizeof(struct test_struct));
	if (result != RING_BUFFER_OK) {
		puts("Failed to initialize ring buffer.");
		exit(EXIT_FAILURE);
	}

	struct test_struct out;
	for (int j = 0; j < 40; j++) {
		for (int i = 0; i < CAPACITY; i++) {
			ring_buffer_push(buffer, &in);
		}

		for (int j = 0; j < CAPACITY; j++) {
			ring_buffer_pop(buffer, &out);
		}
	}

	printf("Reading from `out` to prevent optimization: %f", out.some_double);
	return EXIT_SUCCESS;
}
