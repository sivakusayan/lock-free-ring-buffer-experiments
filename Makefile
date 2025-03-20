##========================
# Compiler Settings
##========================

SRC_DIR = src
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
	override CFLAGS += -g -O3 -DNDEBUG
	OUT_DIR := release
endif

##========================
# Build Recipes
##========================

all: ${OUT_DIR}/ring-buffer.a ${OUT_DIR}/ring-buffer.so

${OUT_DIR}/ring-buffer.a: ${SRC_DIR}/ring-buffer.c
	mkdir -p ${OUT_DIR}
	clang ${CFLAGS} -flto $^ -c -o ${OUT_DIR}/ring-buffer.o
	ar -r ${OUT_DIR}/ring-buffer.a ${OUT_DIR}/ring-buffer.o

${OUT_DIR}/ring-buffer.so: ${SRC_DIR}/ring-buffer.c
	mkdir -p ${OUT_DIR}
	clang -fPIC -shared ${CFLAGS} $^ -o ${OUT_DIR}/ring-buffer.so

${OUT_DIR}/test-runner: ${OUT_DIR}/ring-buffer.so ${SRC_DIR}/test-runner.c ${SRC_DIR}/ring-buffer-test.c
	clang ${CFLAGS} $^ -o ${OUT_DIR}/test-runner

${OUT_DIR}/benchmark: ${SRC_DIR}/benchmark.c ${OUT_DIR}/ring-buffer.a
	clang -static ${CFLAGS} -flto $^ -o ${OUT_DIR}/benchmark

test: ${OUT_DIR}/test-runner
	./${OUT_DIR}/test-runner

benchmark: ${OUT_DIR}/benchmark

clean:
	rm -rf debug
	rm -rf release

.PHONY: all test benchmark clean
