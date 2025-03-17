##========================
# Compiler Settings
##========================

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

##========================
# Build Recipes
##========================

${OUT_DIR}/ring-buffer.so: ring-buffer.c
	mkdir -p ${OUT_DIR}
	clang -fPIC -shared ${CFLAGS} $^ -o ${OUT_DIR}/ring-buffer.so

${OUT_DIR}/test-runner: ${OUT_DIR}/ring-buffer.so test-runner.c ring-buffer-test.c
	clang ${CFLAGS} $^ -o ${OUT_DIR}/test-runner

test: ${OUT_DIR}/test-runner
	./${OUT_DIR}/test-runner

clean:
	rm -rf debug
	rm -rf release

.PHONY: test clean
