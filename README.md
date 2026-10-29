# P01 Part 1: Build Your Own std::vector

Implement every function in `Vector.cpp`. It is the only file you edit and the only file you submit.

## Files

| File | What it is |
|---|---|
| `Vector.h` | Class declaration and the rules for each function. Read it, but do not edit it (Gradescope uses its own copy). |
| `Vector.cpp` | Stub implementations. **Your work goes here.** It is `#include`d by `Vector.h`, not compiled on its own. |
| `main.cpp` | Scratch space for experimenting. Not graded. |
| `tests/tests.cpp` | The test suite (same as Gradescope). Read it to see exactly what is checked. |
| `tests/harness.h` | Test framework: counts heap allocations, defines the `Tracker` element type. |
| `run_tests.py` | Builds and runs each test in its own process. |

## Building and testing

```bash
make test                          # build and run all tests (same as Gradescope)
python3 run_tests.py 15            # run only test 15
make warnings                      # show compiler warnings in Vector.cpp (there must be none)
./vector_tests --list              # list test IDs, points, and names
./vector_tests 15                  # run test 15 directly
gdb --args ./vector_tests 15       # debug test 15 (on a Mac: lldb -- ./vector_tests 15)
make main && ./main                # run your scratch program
```

The build uses AddressSanitizer, which stops your program the moment it touches freed or
out-of-bounds memory. When a test fails with an AddressSanitizer report, the first few lines say
what happened (`heap-use-after-free`, `double-free`, ...) and which line of `Vector.cpp` did it.

If your machine's g++ doesn't have the AddressSanitizer library (the Makefile checks), `make test`
automatically runs each test under valgrind instead, so memory errors still fail the test.
If you move between machines, run `make clean` first.

To use valgrind yourself, build without sanitizers: `make clean && make SANITIZE=`

## Rules

- Do not edit `Vector.h`. You may add non-member helper functions in `Vector.cpp`.
- Allocate with `new T[...]`, free with `delete[]`.
- Do not use `std::vector`, `std::deque`, `std::list`, `std::array`, or smart pointers.
- Whenever `capacity()` is 0, no buffer is allocated and `data()` returns `nullptr`.
