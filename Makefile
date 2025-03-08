test: bin/test-runner
	./bin/test-runner

bin/test-runner: test-runner.c ring-buffer.c ring-buffer-test.c
	mkdir -p bin
	clang test-runner.c ring-buffer.c ring-buffer-test.c -o bin/test-runner

clean:
	rm -rf bin
