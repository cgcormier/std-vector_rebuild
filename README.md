# `Vector<T>`: a C++17 dynamic array

A templated contiguous container implemented with `new[]`, `delete[]`, and three pointers. This project explores the mechanics behind `std::vector`: owning heap memory, separating size from capacity, growing a buffer without losing elements, and making copies independent.

Developed for **UCSB CS32, Project 1 Part 1**. My implementation is in [Vector.cpp](Vector.cpp); the interface, starter scaffolding, test suite, and build tools were supplied by the course.

**Verified:** 48/48 supplied checks passed, with a local test score of 100/100, under AddressSanitizer and UndefinedBehaviorSanitizer on Ubuntu WSL. This includes the compiler-warning check. See the [validation record](docs/validation.md) for the environment, source revision, commands, and limits of that result.

## What this demonstrates

| Area | Work demonstrated in this repository |
| --- | --- |
| Memory ownership and RAII | Allocate and replace a contiguous heap buffer; release owned storage in the destructor; handle empty and allocated-but-empty states. |
| Copy semantics | Implement the Rule of Three, independent deep copies, self-assignment checks, and chained assignment. |
| Data structures and complexity | Separate size from capacity; use configurable geometric growth; preserve elements during reallocation; shift elements for insertion and erasure. |
| Generic C++ | Implement `Vector<T>`, const access, references, overloaded operators, and pointer iterators that work with range-for loops and `std::sort`. |
| Debugging and validation | Interpret sanitizer stack traces, reason about bounds and lifetimes, and validate against allocation-counting and differential stress tests. |

The [engineering notes](docs/engineering-notes.md) connect these skills to specific functions, failure cases, and tradeoffs.

## Try it

Use Linux, WSL, or CSIL with a C++17 compiler, GNU Make, and Python 3. On other systems the supplied runner may need platform-specific tooling; the verified environment is Linux.

```sh
git clone https://github.com/cgcormier/std-vector_rebuild.git
cd std-vector_rebuild
make test
make warnings
```

`make test` builds the program and runs each test in a separate process. The Makefile enables ASan/UBSan when supported; the runner falls back to Valgrind when available and warns if neither memory checker exists.

Run one test while investigating a failure:

```sh
make vector_tests
./vector_tests --list
python3 run_tests.py 13
```

Test IDs refer to the supplied suite. The Python runner rebuilds as needed; running `./vector_tests 13` directly does not rebuild it.

## Example: aliases, algorithms, and independent copies

```cpp
#include <algorithm>
#include "Vector.h"

int main() {
    Vector<int> values{4, 1, 3};
    values.push_back(values[0]);  // Copy an existing element while growing.
    values.insert(1, 2);
    std::sort(values.begin(), values.end());

    Vector<int> copied = values;
    copied[0] = 99;              // values[0] is still 1.
}
```

The [runnable example](examples/usage.cpp) also exercises const iteration, erasure, and buffer compaction:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -I. examples/usage.cpp -o /tmp/vector-demo
/tmp/vector-demo
```

Expected output:

```text
sorted: 1 2 3 4 4
original first: 1, copy first: 99
after erase: 1 3 4 4
size: 4, capacity: 4
```

`Vector.h` includes the template implementation. Compile the program using it, rather than compiling `Vector.cpp` separately.

## Implementation highlights

- **Three-pointer representation:** `begin_` marks the buffer start, `end_` marks one past the logical elements, and `end_cap_` marks one past the allocated slots.
- **Explicit capacity management:** `reserve` grows without changing size; `resize` changes size and initializes added elements; `shrink_to_fit` replaces excess storage.
- **Configurable growth:** full buffers grow to `max(capacity + 1, size_t(capacity * growth_factor))`. With the default factor of 2, repeated append has amortized constant-time element-copy cost; a single growth still takes linear work.
- **Aliasing protection:** `push_back(v[0])` and `insert(0, v.back())` save the incoming value before reallocation or shifting can invalidate or change its source.
- **Correct shift direction:** insertion moves elements from back to front; erasure moves them from front to back.

## Evidence beyond a final score

The course-provided suite checks exact capacities and allocation counts, not only returned values. It includes:

- **100,000 appends:** preserves every value and checks the expected 18 allocations under doubling.
- **20,000 seeded mixed operations:** compares size and contents against `std::vector` after each operation.
- **Ownership checks:** deep copies, nested vectors, self-assignment, allocation release, and tracked object lifetimes.
- **Boundary checks:** empty vectors, invalid indices, reused slots after resize, custom growth factors, and values that alias the container.
- **Integration checks:** `std::sort`, range-for loops, const access, and `std::string` elements.

Those tests were supplied by the course. My contribution was implementing the container and using the failures to debug its behavior.

## Scope and tradeoffs

This is an educational implementation of a selected `std::vector`-like API. Its storage model allocates `T[]`, so class-type objects exist in all capacity slots. Removing logical elements does not immediately destroy those objects; releasing the buffer does. Element types must support the default construction and copying used by these operations.

It has no move operations, allocator customization, or strong exception-safety guarantee for throwing element copies. Reallocation invalidates pointers and references into the previous buffer. Passing the supplied suite establishes the tested behavior, not full standard-library conformance or a performance advantage over `std::vector`.

## Repository guide

| Path | Role |
| --- | --- |
| [Vector.cpp](Vector.cpp) | Container implementation. |
| [Vector.h](Vector.h) | Course-provided interface and function contracts. |
| [tests/](tests/) | Course-provided behavioral suite and allocation/lifetime harness. |
| [Makefile](Makefile), [run_tests.py](run_tests.py) | Course-provided build, sanitizer detection, and test runner. |
| [main.cpp](main.cpp) | Supplied scratch program that prints size/capacity growth. |
| [examples/usage.cpp](examples/usage.cpp) | Runnable demonstration added for this portfolio. |
| [docs/engineering-notes.md](docs/engineering-notes.md) | Design decisions, debugging lessons, and evidence of skills gained. |
| [docs/validation.md](docs/validation.md) | Reproducible validation record and limitations. |

Build outputs and editor swap files stay local. The supplied starter history is retained, and course-owned scaffolding is credited rather than presented as my original test infrastructure.
