# Validation record

Validated on **2026-11-01 UTC (2026-10-31 Pacific)** using a fresh temporary copy of the source in Ubuntu WSL. The executable downloaded from CSIL was preserved locally and was not used for these checks.

## Source and environment

| Item | Verified value |
| --- | --- |
| Implementation revision | [`1dff0df`](https://github.com/cgcormier/std-vector_rebuild/commit/1dff0dff82d0014ffd575fa425c4eecb8c12fcf9) |
| `Vector.cpp` SHA-256 | `3ad4b16dcb93778b5b8b036d3f8cd40f1ab6714cadd5be54b7fea0856a0503a6` |
| Operating system | Ubuntu 26.04 LTS on WSL2; Linux 6.18.33.1-microsoft-standard-WSL2 |
| Compiler | GCC 15.2.0, Ubuntu package 15.2.0-16ubuntu1 |
| Language and warning flags | `-std=c++17 -Wall -Wextra -pedantic -g -O0` |
| Memory checking | `-fsanitize=address,undefined -fno-omit-frame-pointer` |
| Test runner | Python 3.14.4 with the course-provided `run_tests.py` |

The documentation commit adds the usage example and this report without changing the tested implementation. The source hash identifies exactly which `Vector.cpp` was validated.

## Commands and results

From a clean source copy:

```sh
make -B vector_tests
python3 run_tests.py --json results.json
make warnings
```

Result:

```text
Passed 48/48 tests. Score: 100/100
```

The 48 checks consist of **47 C++ behavioral tests and one compiler-warning check**. The separate `make warnings` invocation also completed without warnings. Both ASan and UBSan were enabled in the actual compile command; this was not a run with memory checking disabled.

The runner's JSON was checked for a full score and for every test having `status == "passed"`. This matters because a completed runner process alone does not establish that every test passed.

The supplied scratch program was also compiled and run:

```sh
make main
./main
```

Its ten appends reported capacities `1, 2, 4, 4, 8, 8, 8, 8, 16, 16` as size increased from 1 to 10.

## Portfolio example

The new [example](../examples/usage.cpp) was compiled with the same warning and sanitizer flags:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -g -O0 \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    -I. examples/usage.cpp -o /tmp/vector-demo
/tmp/vector-demo
```

Its assertions passed and its output matched the README exactly:

```text
sorted: 1 2 3 4 4
original first: 1, copy first: 99
after erase: 1 3 4 4
size: 4, capacity: 4
```

This exercises a self-referencing append that reallocates, insertion, sorting through pointer iterators, const iteration, independent deep copies, erasure, and compaction. The separate C++ snippet in the README was extracted, compiled with sanitizers, and run successfully as well.

## Coverage and limitations

The supplied suite includes exact allocation-size/count checks, custom growth factors, a 100,000-append test, tracked-object and nested-vector tests, and a deterministic 20,000-operation comparison against `std::vector`.

The runner sets `ASAN_OPTIONS=detect_leaks=0:allocator_may_return_null=0` and `UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1`. ASan leak detection is therefore disabled; allocation/release checks come from the supplied harness. No sanitizer errors were reported in these exercised paths.

The original downloaded implementation already passed all 48 checks. Source review then added an explicit null guard in `size()` to avoid subtracting null pointers for unallocated vectors; the suite was rerun successfully afterward. Stale starter comments and whitespace were also cleaned up.

These are local validation results, not a Gradescope submission or a claim of complete `std::vector` conformance. Throwing element operations, extreme-size overflow, and cross-platform behavior beyond the stated environment were not validated. No comparative timing benchmark or automated CI result is claimed. See the [engineering notes](engineering-notes.md) for the storage-model and exception-safety tradeoffs.
