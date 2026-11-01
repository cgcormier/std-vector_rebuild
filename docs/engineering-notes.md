# Engineering notes

This project implements a dynamic array for UCSB CS32 Project 1 Part 1. The learning goal was to turn the supplied contracts into working C++ code and understand the memory model behind familiar container operations.

## Ownership and the three-pointer model

The vector object owns one heap allocation. Its three pointers describe two different ranges:

| Pointer | Boundary |
| --- | --- |
| `begin_` | First slot in the allocation. |
| `end_` | One past the last logical element. |
| `end_cap_` | One past the last allocated slot. |

For allocated storage, size is `end_ - begin_` and capacity is `end_cap_ - begin_`. A vector can be empty while retaining capacity: `begin_ == end_` does not imply either pointer is null. With no allocation, all three are null and size/capacity report zero.

This distinction affects `front`, `back`, `reserve`, `clear`, and resizing. Having memory available does not mean a first or last logical element exists. Accessors returning `T&` must identify an existing element; they cannot return a temporary or dereference a boundary pointer.

Source: [size and capacity](../Vector.cpp#L246), [buffer replacement](../Vector.cpp#L138), and [destructor](../Vector.cpp#L129).

## Reallocation and growth

`reallocate(new_cap)` allocates a replacement buffer, copies the current logical elements, releases the old buffer, and reconstructs the boundaries. Its precondition is `new_cap >= size()`.

The number of elements to copy comes from the old **size**, not the new capacity. When a full one-slot buffer grows to two slots, the old source still contains only one element. Copying two elements reads outside that source allocation, regardless of the destination's extra room.

The growth formula is:

```text
new capacity = max(capacity + 1, size_t(capacity * growth_factor))
```

The default factor of 2 produces capacities 1, 2, 4, 8, and so on. Under fixed geometric growth, the total element transfers across repeated appends form a geometric series, giving amortized O(1) append work for constant-cost element operations. That does not make every append O(1): an append that reallocates has linear work. A factor of 1 grows by one each time and loses that amortized benefit.

The tests use growth factors 1.0, 1.5, and 4.0 as well as the default. A [100,000-append case](../tests/tests.cpp#L217) verifies 18 allocations and retained values; this is an allocation-behavior check, not a timing benchmark.

Source: [push_back](../Vector.cpp#L298).

## Deep copies and assignment

The copy constructor allocates its own buffer. Changing the copy therefore cannot change the original's elements. Assignment must additionally release the destination's previous allocation and handle assigning an object to itself.

Two ideas are easy to confuse:

- Equal contents do not mean two vectors are the same object. Self-assignment is checked using object addresses: `this == &other`.
- Assignment replaces the destination's contents. The source object and its allocation remain intact.

The current assignment implementation shrinks its logical size first when needed so that its subsequent call to `reallocate(other.size())` satisfies the helper's precondition. It then copies the source elements and returns `*this` to support `a = b = c`.

The implementation follows the Rule of Three: destructor, copy constructor, and copy assignment. It does not implement move semantics or copy-and-swap.

Source: [copy constructor](../Vector.cpp#L91), [assignment](../Vector.cpp#L107), and [copy tests](../tests/tests.cpp#L448).

## References that alias the buffer

A call such as `v.push_back(v[0])` passes a reference into storage the function might replace. If the old allocation is freed before that reference is read, the read uses freed memory.

Both append and insertion preserve the incoming value in a local `T` before altering storage. Insertion also needs this protection without reallocation: shifting elements could overwrite the element referred to by `value`.

This connects references, lvalues, and lifetime to a concrete container behavior. A non-null pointer alone cannot establish that an index is valid or that a reference still points into a live allocation.

Source: [aliasing tests](../tests/tests.cpp#L613).

## In-place insertion and erasure

Insertion creates a gap by shifting a suffix one slot right. The implementation works backward, copying the last element into the spare slot before moving earlier elements. Forward copying would overwrite a source before its turn.

Erasure shifts a suffix left. Forward copying is safe in that direction because each destination is behind the unread sources. Both operations update the logical end while preserving capacity unless insertion first requires growth.

The important reasoning step is choosing a traversal order based on which values still need to be read, rather than changing index arithmetic until a test passes.

Source: [insert](../Vector.cpp#L357) and [erase](../Vector.cpp#L390).

## Debugging lessons applied

| Failure or misconception | Reasoning used to resolve it |
| --- | --- |
| ASan reports a read just after a one-integer allocation. | Locate the first implementation frame, distinguish a read from a write, then compare the source index with the old buffer's bounds. |
| Dereferencing `end_` to obtain the last element. | `end_` is a boundary; access the preceding element after checking emptiness, without changing the stored end pointer. |
| Checking only for a null buffer before accessing an element. | An allocated vector can still have size zero. Check the function's logical precondition. |
| `n - size()` becomes a huge loop bound when shrinking. | `std::size_t` is unsigned. Only initialize a newly added range when `n > size()`. |
| Calling `at()` through a const reference does not compile. | The supplied `at()` has no const overload; use the supplied const subscript operator where bounds are already established. |
| Editing code but rerunning an old executable. | Save first, rebuild, and run only after a successful build; the Python runner performs the build check. |

The publication review also added an explicit null guard to `size()`: an unallocated empty vector returns zero without subtracting null pointers. The supplied suite passed before and after that change; source review identified a portability concern the runtime checks did not report.

## What the tests establish

The supplied harness counts allocations and releases and includes a `Tracker` type for object-lifetime checks. The suite covers `int`, `std::string`, tracked objects, and nested vectors. Its seeded [20,000-operation differential test](../tests/tests.cpp#L687) compares the implementation against `std::vector` after each operation.

ASan and UBSan add checks for the exercised memory accesses and undefined behavior. They do not prove every possible operation or element type is safe. ASan's leak detector is disabled by the runner; the supplied allocation counters check for unreleased allocations in the tested cases.

## Tradeoffs and current limits

- `new T[capacity]` constructs class-type objects in every slot. Logical removal does not destroy those objects immediately. This follows the course storage model and differs from `std::vector`'s element-lifetime management.
- Default construction and copying are required by the allocation and assignment strategy. Move-only types are not supported by this implementation. Element comparison uses `T::operator!=` or an equivalent free operator.
- Copying elements that throw is not exception-safe in every path; constructors and reallocation do not use a temporary RAII owner for partially completed copies. No strong exception guarantee is claimed.
- A capacity change invalidates existing pointers, references, and iterators into the old allocation. Without growth, insertion and erasure can change the values at and after the affected position.
- No allocator interface, move constructor, move assignment, or full standard-library API is implemented. Extremely large sizes and overflow in growth calculations are outside the supplied validation.
- Complexity discussions count element operations. Default-constructing and destroying capacity slots can add work for class types. No performance benchmark against the standard library has been run.

## Experience gained and resume wording

This project gave me practice translating API contracts into implementation, tracing pointer lifetimes, explaining amortized growth, debugging memory errors, and validating generic code against explicit behavioral and allocation expectations.

Claims supported by this repository:

- Implemented a C++17 templated dynamic array with manual heap ownership, deep-copy semantics, configurable growth, and pointer iterators compatible with `std::sort`.
- Validated the implementation against 48 supplied checks under ASan/UBSan, including allocation-counting tests and a seeded 20,000-operation comparison with `std::vector`.
- Debugged buffer-boundary, aliasing, and unsigned-index issues while implementing insertion, erasure, and capacity management.

These describe a coursework implementation and its validation. They do not claim authorship of the supplied testing infrastructure or production deployment.
