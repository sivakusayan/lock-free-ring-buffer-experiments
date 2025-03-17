override CFLAGS += -Wall -Wextra -Wconversion
ifeq ($(DEBUG), 1)
	override CFLAGS += -g3
	OUT_DIR := debug
	# Apparently the leak sanitizer isn't support on ARM processors yet.
	ifeq ($(shell arch), arm64)
		override CFLAGS += -fsanitize=address,undefined
	else
		override CFLAGS += -fsanitize=address,undefined,leak
	endif
else
	override CFLAGS += -g -O3
	OUT_DIR := release
endif

test: ${OUT_DIR}/test-runner
	./${OUT_DIR}/test-runner

${OUT_DIR}/test-runner: test-runner.c ring-buffer.c ring-buffer-test.c
	mkdir -p ${OUT_DIR}
	clang ${CFLAGS} test-runner.c ring-buffer.c ring-buffer-test.c -o ${OUT_DIR}/test-runner

clean:
	rm -rf debug
	rm -rf release

.PHONY: test clean
