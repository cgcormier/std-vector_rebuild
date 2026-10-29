CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -O0

# AddressSanitizer + UndefinedBehaviorSanitizer catch use-after-free, double
# free, and out-of-bounds accesses the moment they happen. Some machines have
# g++ but not the sanitizer libraries, so we only turn them on if a tiny test
# program links with them. Without them, `make test` runs the tests under
# valgrind instead. To turn them off yourself:  make clean && make SANITIZE=
SANITIZER_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer
SANITIZE ?= $(shell tmp=$$(mktemp) && \
    echo 'int main() { return 0; }' | $(CXX) -x c++ $(SANITIZER_FLAGS) - -o $$tmp >/dev/null 2>&1 && \
    echo '$(SANITIZER_FLAGS)'; rm -f $$tmp)

all: vector_tests main

vector_tests: tests/tests.cpp tests/harness.h Vector.h Vector.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZE) -o $@ tests/tests.cpp

main: main.cpp Vector.h Vector.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZE) -o $@ main.cpp

test:
	python3 run_tests.py

# Used by run_tests.py to decide whether to fall back to valgrind.
print-sanitize:
	@echo '$(SANITIZE)'

# Checks your code for compiler warnings without building anything. Always
# re-checks, even if nothing changed. `make test` runs this as a graded test.
warnings:
	$(CXX) $(CXXFLAGS) -fsyntax-only tests/tests.cpp

clean:
	rm -rf vector_tests main *.dSYM

.PHONY: all test clean print-sanitize warnings
