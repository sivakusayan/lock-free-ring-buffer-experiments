// Test runner idea taken and adapted from this reddit comment
// https://www.reddit.com/r/C_Programming/comments/vfm3s7/comment/icwsoac/?utm_source=share&utm_medium=web2x&context=3
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* test = "";
const char* test_group = "";
const char* assertion = "";
const char* file = "";
int line = 0;

static void fail(void)
{
	if (strcmp(test_group, "") == 0) {
		printf(
			"!\nTest failed at %s:%d\n\t%s: %s\n", file, line, test, assertion);
	} else {
		printf("!\nTest failed at %s:%d\n\t[%s] %s: %s\n",
			   file,
			   line,
			   test_group,
			   test,
			   assertion);
	}
	exit(1);
}

#define TESTGROUP(name) test_group = name;
#define ENDTESTGROUP()	test_group = "";
#define TEST(name)		test = name;
#define ASSERT(ast)       \
	do {                  \
		assertion = #ast; \
		file = __FILE__;  \
		line = __LINE__;  \
		if (ast)          \
			putchar('.'); \
		else              \
			fail();       \
	} while (0)

#define HEADERS
#include "ring-buffer-test.c"
#undef HEADERS

int main(void)
{
#define TESTS
#include "ring-buffer-test.c"
#undef TESTS

	puts("\nAll tests passed.");
	return 0;
}
