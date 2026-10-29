// P01 Part 1 test suite. These are the same tests Gradescope runs.
//
// Run everything:        make test
// Run one test:         python3 run_tests.py 15
// List test IDs:         ./vector_tests --list
// Run one test directly: ./vector_tests 15
//   (with a debugger:    gdb --args ./vector_tests 15   or   lldb -- ./vector_tests 15)
//
// A test's ID is its position in this file, starting at 1. Tests are grouped
// by the writeup's checkpoints, in the order you implement the functions.
//
// alloc::Snapshot counts heap allocations made after it was created.
// Tracker counts constructions/copies/destructions of elements.

#include "harness.h"
#include "../Vector.h"

#include <algorithm>
#include <random>
#include <string>
#include <vector>  // used only by the tests as a reference; Vector.cpp may not use it

// Builds a Vector<int> containing 0, 1, ..., n-1 via push_back.
static Vector<int> make_range(int n) {
    Vector<int> v;
    for (int i = 0; i < n; ++i) v.push_back(i);
    return v;
}

#define check_contents(v, ...) check_contents_at(__FILE__, __LINE__, v, __VA_ARGS__)  // reports the caller's line
template <typename V>
static void check_contents_at(const char* file, int line, const V& v, std::initializer_list<int> expected) {
    if (v.size() != expected.size())
        harness::fail(file, line, "size is " + std::to_string(v.size()) + ", expected " + std::to_string(expected.size()));
    std::size_t i = 0;
    for (int e : expected) {
        if (!harness::equal(v[i], e))
            harness::fail(file, line, "element " + std::to_string(i) + " is " + harness::show(v[i]) + ", expected " + std::to_string(e));
        ++i;
    }
}

// ============================================================================
// Checkpoint 1: constructors, element access, iterators
// ============================================================================

TEST(ctor_default, "[Constructors] Default constructor: empty, no allocation", 2) {
    CHECK_EQ(sizeof(Vector<int>), 3 * sizeof(int*));   // exactly three pointers
    alloc::Snapshot s;
    Vector<int> v;
    CHECK_EQ(v.size(), 0);
    CHECK_EQ(v.capacity(), 0);
    CHECK(v.empty());
    CHECK(v.data() == nullptr);
    CHECK_EQ(s.allocs(), 0);
}


TEST(ctor_size, "[Constructors] Vector(n): n zeros, one allocation", 2) {
    alloc::Snapshot s;
    Vector<int> v(50);
    CHECK_EQ(v.size(), 50);
    CHECK_EQ(v.capacity(), 50);
    CHECK(!v.empty());
    for (std::size_t i = 0; i < v.size(); ++i) CHECK_EQ(v[i], 0);
    CHECK_EQ(s.allocs(), 1);
    CHECK_EQ(s.alloc_bytes(0), 50 * sizeof(int));
}


TEST(ctor_size_zero, "[Constructors] Vector(0) and Vector{} allocate nothing", 2) {
    alloc::Snapshot s;
    Vector<int> a(0);
    Vector<int> b(0, 7);
    Vector<int> c{};
    CHECK_EQ(a.capacity(), 0);
    CHECK_EQ(b.capacity(), 0);
    CHECK_EQ(c.capacity(), 0);
    CHECK(a.data() == nullptr);
    CHECK(b.data() == nullptr);
    CHECK_EQ(s.allocs(), 0);
}


TEST(ctor_fill, "[Constructors] Vector(n, value)", 2) {
    alloc::Snapshot s;
    Vector<int> v(1000, 1);
    CHECK_EQ(v.size(), 1000);
    CHECK_EQ(v.capacity(), 1000);
    for (std::size_t i = 0; i < v.size(); ++i) CHECK_EQ(v[i], 1);
    CHECK_EQ(s.allocs(), 1);

    Vector<std::string> w(3, "gaucho");
    CHECK_EQ(w.size(), 3);
    CHECK_EQ(w[2], std::string("gaucho"));
}


TEST(ctor_ilist, "[Constructors] Initializer list constructor", 2) {
    alloc::Snapshot s;
    Vector<int> v{1, 2, 3};
    check_contents(v, {1, 2, 3});
    CHECK_EQ(v.capacity(), 3);
    CHECK_EQ(s.allocs(), 1);
}


TEST(access_subscript, "[Access] operator[] reads and writes", 1) {
    Vector<int> v{10, 20, 30};
    CHECK_EQ(v[0], 10);
    CHECK_EQ(v[2], 30);
    v[1] = 99;
    check_contents(v, {10, 99, 30});
}


TEST(access_at, "[Access] at() reads and writes in range", 1) {
    Vector<int> v{10, 20, 30};
    CHECK_EQ(v.at(0), 10);
    CHECK_EQ(v.at(2), 30);
    v.at(1) = 99;
    check_contents(v, {10, 99, 30});
}


TEST(access_at_oob, "[Access] at() throws std::out_of_range when out of bounds", 2) {
    Vector<int> v{10, 20, 30};
    CHECK_THROWS(v.at(3), std::out_of_range);
    CHECK_THROWS(v.at(1000000), std::out_of_range);
    Vector<int> e;
    CHECK_THROWS(e.at(0), std::out_of_range);
}


TEST(access_front_back, "[Access] front() and back()", 1) {
    Vector<int> v{4, 5, 6};
    CHECK_EQ(v.front(), 4);
    CHECK_EQ(v.back(), 6);
    v.front() = 1;
    v.back() = 9;
    check_contents(v, {1, 5, 9});
    Vector<int> one{42};
    CHECK_EQ(one.front(), 42);
    CHECK_EQ(one.back(), 42);
}


TEST(access_const, "[Access] const Vector supports [], range-for, and size queries", 1) {
    const Vector<int> v{7, 8, 9};
    CHECK_EQ(v[1], 8);
    CHECK_EQ(v[2], 9);
    CHECK_EQ(v.size(), 3);
    CHECK_EQ(v.capacity(), 3);
    CHECK(!v.empty());
    int sum = 0;
    for (const int& x : v) sum += x;
    CHECK_EQ(sum, 24);
}


TEST(iter_range_for, "[Iterators] range-for reads and modifies elements", 2) {
    Vector<int> v{1, 2, 3, 4, 5};
    int sum = 0;
    for (int x : v) sum += x;
    CHECK_EQ(sum, 15);
    for (int& x : v) x *= 10;
    check_contents(v, {10, 20, 30, 40, 50});
}


TEST(iter_std_sort, "[Iterators] works with std::sort", 1) {
    Vector<int> v{5, 3, 9, 1, 7, 2};
    std::sort(v.begin(), v.end());
    check_contents(v, {1, 2, 3, 5, 7, 9});
}


// ============================================================================
// Checkpoint 2: reallocate + push_back, pop_back, reserve, clear, shrink_to_fit, resize
// ============================================================================

TEST(growth_sequence, "[Growth] push_back grows capacity 1, 2, 4, 8, 16", 4) {
    alloc::Snapshot s;
    Vector<int> v;
    const std::size_t expected_caps[] = {1, 2, 4, 4, 8, 8, 8, 8, 16};
    int expected_allocs = 0;
    std::size_t prev_cap = 0;
    for (int i = 0; i < 9; ++i) {
        v.push_back(i);
        CHECK_EQ(v.size(), i + 1);
        CHECK_EQ(v.capacity(), expected_caps[i]);
        if (v.capacity() != prev_cap) {
            // a reallocation must request exactly capacity * sizeof(int) bytes
            CHECK_EQ(s.alloc_bytes(expected_allocs), v.capacity() * sizeof(int));
            ++expected_allocs;
        }
        CHECK_EQ(s.allocs(), expected_allocs);
        prev_cap = v.capacity();
    }
    check_contents(v, {0, 1, 2, 3, 4, 5, 6, 7, 8});
}


TEST(growth_frees_old, "[Growth] Old buffer is freed after each reallocation", 3) {
    alloc::Snapshot s;
    {
        Vector<int> v;
        for (int i = 0; i < 9; ++i) {
            v.push_back(i);
            CHECK_EQ(s.live_delta(), 1);   // only the current buffer is alive
        }
    }
    CHECK_EQ(s.live_delta(), 0);
}


TEST(growth_many, "[Growth] 100,000 push_backs: values kept, 18 allocations", 2) {
    alloc::Snapshot s;
    Vector<int> v;
    for (int i = 0; i < 100000; ++i) v.push_back(i * 3);
    CHECK_EQ(v.size(), 100000);
    CHECK_EQ(v.capacity(), 131072);
    CHECK_EQ(s.allocs(), 18);   // capacities 1, 2, 4, ..., 131072
    CHECK_EQ(s.live_delta(), 1);
    for (int i = 0; i < 100000; ++i) {
        if (v[i] != i * 3) CHECK_EQ(v[i], i * 3);
    }
}


TEST(growth_factor_1_5, "[Growth] growth_factor = 1.5 gives 1, 2, 3, 4, 6, 9, 13, 19", 2) {
    Vector<int>::growth_factor = 1.5;
    Vector<int> v;
    const std::size_t expected_caps[] = {1, 2, 3, 4, 6, 9, 13, 19};
    std::size_t i = 0;
    std::size_t prev_cap = 0;
    for (int n = 0; n < 19; ++n) {
        v.push_back(n);
        if (v.capacity() != prev_cap) {
            CHECK(i < 8);
            CHECK_EQ(v.capacity(), expected_caps[i]);
            ++i;
            prev_cap = v.capacity();
        }
    }
    CHECK_EQ(i, 8);
    CHECK_EQ(v[18], 18);
    Vector<int>::growth_factor = 2.0;
}


TEST(growth_factor_other, "[Growth] growth_factor = 1.0 grows by one; 4.0 gives 1, 4, 16, 64", 2) {
    Vector<int>::growth_factor = 1.0;
    {
        alloc::Snapshot s;
        Vector<int> v;
        for (int n = 1; n <= 10; ++n) {
            v.push_back(n);
            CHECK_EQ(v.capacity(), n);
        }
        CHECK_EQ(s.allocs(), 10);
    }
    Vector<int>::growth_factor = 4.0;
    {
        Vector<int> v;
        const std::size_t expected_caps[] = {1, 4, 4, 4, 16};
        for (int n = 0; n < 5; ++n) {
            v.push_back(n);
            CHECK_EQ(v.capacity(), expected_caps[n]);
        }
        for (int n = 5; n < 17; ++n) v.push_back(n);
        CHECK_EQ(v.capacity(), 64);
    }
    Vector<int>::growth_factor = 2.0;
}


TEST(growth_tracker, "[Growth] Elements survive reallocation; all are destroyed at the end", 2) {
    {
        Vector<Tracker> v;
        for (int i = 0; i < 20; ++i) v.push_back(Tracker(i));
        CHECK_EQ(v.size(), 20);
        for (int i = 0; i < 20; ++i) CHECK_EQ(v[i].value, i);
    }
    CHECK_EQ(Tracker::alive, 0);
}


TEST(iter_pointers, "[Iterators] begin() == data(), end() - begin() == size()", 1) {
    Vector<int> v{1, 2, 3, 4};
    CHECK(v.begin() == v.data());
    CHECK_EQ(v.end() - v.begin(), 4);
    Vector<int> e;
    CHECK(e.begin() == e.end());
    v.push_back(5);   // grows to capacity 8
    CHECK(v.begin() == v.data());
    CHECK_EQ(v.end() - v.begin(), 5);
}


TEST(mod_pop_back, "[Modifiers] pop_back removes the last element, keeps capacity", 2) {
    Vector<int> v = make_range(5);
    alloc::Snapshot s;
    v.pop_back();
    v.pop_back();
    check_contents(v, {0, 1, 2});
    CHECK_EQ(v.capacity(), 8);
    CHECK(!v.empty());
    v.pop_back();
    v.pop_back();
    v.pop_back();
    CHECK_EQ(v.size(), 0);
    CHECK(v.empty());               // empty even though the buffer is still allocated
    CHECK(v.data() != nullptr);
    CHECK_EQ(v.capacity(), 8);
    CHECK_EQ(s.allocs(), 0);
    CHECK_EQ(s.frees(), 0);
}


TEST(access_empty_throws, "[Access] front/back/pop_back throw std::out_of_range when empty", 2) {
    Vector<int> v;
    CHECK_THROWS(v.front(), std::out_of_range);
    CHECK_THROWS(v.back(), std::out_of_range);
    CHECK_THROWS(v.pop_back(), std::out_of_range);
}


TEST(reserve_grows, "[Capacity] reserve(n) grows capacity to exactly n", 2) {
    Vector<int> v{1, 2, 3};
    alloc::Snapshot s;
    v.reserve(100);
    CHECK_EQ(v.capacity(), 100);
    check_contents(v, {1, 2, 3});
    CHECK_EQ(s.allocs(), 1);
    CHECK_EQ(s.alloc_bytes(0), 100 * sizeof(int));
    CHECK_EQ(s.frees(), 1);
}


TEST(reserve_noop, "[Capacity] reserve(n) with n <= capacity does nothing", 2) {
    Vector<int> v{1, 2, 3};
    v.reserve(10);
    alloc::Snapshot s;
    v.reserve(10);
    v.reserve(5);
    v.reserve(0);
    CHECK_EQ(v.capacity(), 10);
    check_contents(v, {1, 2, 3});
    CHECK_EQ(s.allocs(), 0);
    CHECK_EQ(s.frees(), 0);
}


TEST(reserve_then_push, "[Capacity] push_back after reserve doesn't reallocate until full", 2) {
    Vector<int> v;
    v.reserve(100);
    alloc::Snapshot s;
    for (int i = 0; i < 100; ++i) v.push_back(i);
    CHECK_EQ(s.allocs(), 0);
    CHECK_EQ(v.capacity(), 100);
    v.push_back(100);
    CHECK_EQ(v.capacity(), 200);
    CHECK_EQ(s.allocs(), 1);
    CHECK_EQ(v[100], 100);
}


TEST(mod_clear, "[Modifiers] clear empties the vector, keeps capacity", 2) {
    Vector<int> v = make_range(5);
    alloc::Snapshot s;
    v.clear();
    CHECK_EQ(v.size(), 0);
    CHECK(v.empty());
    CHECK_EQ(v.capacity(), 8);
    CHECK_EQ(s.frees(), 0);
    v.push_back(42);
    check_contents(v, {42});
    CHECK_EQ(s.allocs(), 0);
}


TEST(shrink_basic, "[Capacity] shrink_to_fit reduces capacity to size", 2) {
    Vector<int> v = make_range(5);   // capacity 8
    alloc::Snapshot s;
    v.shrink_to_fit();
    CHECK_EQ(v.capacity(), 5);
    check_contents(v, {0, 1, 2, 3, 4});
    CHECK_EQ(s.allocs(), 1);
    CHECK_EQ(s.alloc_bytes(0), 5 * sizeof(int));
    CHECK_EQ(s.frees(), 1);
}


TEST(shrink_noop, "[Capacity] shrink_to_fit does nothing when size == capacity", 1) {
    Vector<int> v{1, 2, 3};
    alloc::Snapshot s;
    v.shrink_to_fit();
    CHECK_EQ(v.capacity(), 3);
    CHECK_EQ(s.allocs(), 0);
    CHECK_EQ(s.frees(), 0);
}


TEST(shrink_empty, "[Capacity] shrink_to_fit on an empty vector frees the buffer", 2) {
    Vector<int> v = make_range(5);
    v.clear();
    alloc::Snapshot s;
    v.shrink_to_fit();
    CHECK_EQ(v.capacity(), 0);
    CHECK(v.data() == nullptr);
    CHECK_EQ(s.allocs(), 0);
    CHECK_EQ(s.frees(), 1);
    v.push_back(7);   // still usable afterwards
    check_contents(v, {7});
}


TEST(mod_resize_within, "[Modifiers] resize within capacity; new elements are zero", 2) {
    Vector<int> v{1, 2, 3, 4};
    alloc::Snapshot s;
    v.resize(2);
    check_contents(v, {1, 2});
    CHECK_EQ(v.capacity(), 4);
    v.resize(4);   // the old 3 and 4 must not come back
    check_contents(v, {1, 2, 0, 0});
    v.resize(4);
    check_contents(v, {1, 2, 0, 0});
    CHECK_EQ(s.allocs(), 0);
}


TEST(mod_resize_beyond, "[Modifiers] resize beyond capacity reserves exactly n", 2) {
    Vector<int> v{1, 2};
    v.resize(5);
    check_contents(v, {1, 2, 0, 0, 0});
    CHECK_EQ(v.capacity(), 5);
    v.resize(0);
    CHECK(v.empty());
    CHECK_EQ(v.capacity(), 5);
}


// ============================================================================
// Checkpoint 3: copy constructor and copy assignment
// ============================================================================

TEST(copy_ctor, "[Big 3] Copy constructor makes an independent deep copy", 4) {
    Vector<int> a = make_range(5);   // capacity 8
    alloc::Snapshot s;
    Vector<int> b(a);
    CHECK_EQ(s.allocs(), 1);
    CHECK(b.data() != a.data());
    check_contents(b, {0, 1, 2, 3, 4});
    CHECK_EQ(b.capacity(), 5);   // capacity == other.size()
    b[0] = 100;
    b.push_back(5);
    check_contents(a, {0, 1, 2, 3, 4});
    a[1] = 200;
    CHECK_EQ(b[1], 1);
}


TEST(copy_ctor_empty, "[Big 3] Copying an empty vector allocates nothing", 2) {
    Vector<int> a;
    Vector<int> c = make_range(3);
    c.clear();   // size 0, capacity 4
    alloc::Snapshot s;
    Vector<int> b(a);
    Vector<int> d(c);
    CHECK_EQ(s.allocs(), 0);
    CHECK(b.empty());
    CHECK(d.empty());
    CHECK(d.data() == nullptr);
    CHECK_EQ(d.capacity(), 0);
}


TEST(copy_assign, "[Big 3] Copy assignment deep-copies and frees the old buffer", 4) {
    Vector<int> a{1, 2, 3};
    Vector<int> b = make_range(10);
    alloc::Snapshot s;
    b = a;
    check_contents(b, {1, 2, 3});
    CHECK_EQ(b.capacity(), 3);
    CHECK(b.data() != a.data());
    CHECK_EQ(s.live_delta(), 0);   // one new buffer, one old buffer freed
    b[0] = 50;
    CHECK_EQ(a[0], 1);
    a.push_back(4);
    CHECK_EQ(b.size(), 3);
}


TEST(copy_assign_self, "[Big 3] Self-assignment is safe", 3) {
    Vector<int> a{1, 2, 3};
    alloc::Snapshot s;
    Vector<int>& alias = a;
    a = alias;
    check_contents(a, {1, 2, 3});
    CHECK_EQ(s.live_delta(), 0);
    Vector<std::string> w{"x", "y"};
    Vector<std::string>& walias = w;
    w = walias;
    CHECK_EQ(w.size(), 2);
    CHECK_EQ(w[1], std::string("y"));
}


TEST(copy_assign_chain, "[Big 3] Assignment returns *this (a = b = c works)", 1) {
    Vector<int> a, b;
    Vector<int> c{7, 8};
    a = b = c;
    check_contents(a, {7, 8});
    check_contents(b, {7, 8});
    CHECK(a.data() != b.data());
}


TEST(copy_assign_empty, "[Big 3] Assigning to and from empty vectors", 2) {
    Vector<int> empty;
    Vector<int> a{1, 2, 3};
    alloc::Snapshot s;
    a = empty;
    CHECK(a.empty());
    CHECK_EQ(a.capacity(), 0);
    CHECK(a.data() == nullptr);
    CHECK_EQ(s.live_delta(), -1);   // a's old buffer was freed
    Vector<int> b;
    Vector<int> c{4, 5};
    b = c;
    check_contents(b, {4, 5});
}


TEST(dtor_frees, "[Big 3] Destructor frees the buffer and destroys every element", 3) {
    alloc::Snapshot s;
    {
        Vector<int> v = make_range(100);
        Vector<int> w(v);
        Vector<int> x;
        x = w;
    }
    CHECK_EQ(s.live_delta(), 0);
    {
        Vector<Tracker> t(10, Tracker(5));
        Vector<Tracker> u(t);
        u.push_back(Tracker(6));
    }
    CHECK_EQ(Tracker::alive, 0);
    CHECK_EQ(s.live_delta(), 0);
}


TEST(nested_vectors, "[Big 3] Vector<Vector<int>> copies deeply", 2) {
    alloc::Snapshot s;
    {
        Vector<Vector<int>> grid;
        for (int r = 0; r < 5; ++r) grid.push_back(make_range(r + 1));   // forces regrowth
        Vector<Vector<int>> copy(grid);
        copy[4][0] = 999;
        CHECK_EQ(grid[4][0], 0);
        CHECK_EQ(copy[3].size(), 4);
        grid = copy;
        CHECK_EQ(grid[4][0], 999);
        CHECK(grid[4].data() != copy[4].data());
    }
    CHECK_EQ(s.live_delta(), 0);
}


// ============================================================================
// Checkpoint 4: insert, erase, operator==, and everything together
// ============================================================================

TEST(mod_insert, "[Modifiers] insert at front, middle, and end", 3) {
    Vector<int> v{1, 2, 3};
    v.reserve(10);
    v.insert(0, 0);
    check_contents(v, {0, 1, 2, 3});
    v.insert(2, 99);
    check_contents(v, {0, 1, 99, 2, 3});
    v.insert(v.size(), 4);
    check_contents(v, {0, 1, 99, 2, 3, 4});
    Vector<int> e;
    e.insert(0, 5);
    check_contents(e, {5});
}


TEST(mod_insert_grow, "[Modifiers] insert into a full vector grows by growth_factor", 2) {
    Vector<int> v{1, 2, 3, 4};
    alloc::Snapshot s;
    v.insert(1, 9);
    check_contents(v, {1, 9, 2, 3, 4});
    CHECK_EQ(v.capacity(), 8);
    CHECK_EQ(s.allocs(), 1);
    CHECK_EQ(s.live_delta(), 0);
}


TEST(mod_insert_oob, "[Modifiers] insert past size() throws and leaves the vector unchanged", 2) {
    Vector<int> v{1, 2, 3};
    CHECK_THROWS(v.insert(4, 0), std::out_of_range);
    CHECK_THROWS(v.insert(100, 0), std::out_of_range);
    check_contents(v, {1, 2, 3});
    Vector<int> e;
    CHECK_THROWS(e.insert(1, 0), std::out_of_range);
    CHECK_EQ(e.capacity(), 0);
}


TEST(growth_aliasing, "[Growth] push_back/insert of an element of the same full vector", 2) {
    // value refers into the buffer that is about to be reallocated.
    Vector<int> v{1, 2, 3, 4};   // size == capacity == 4
    v.push_back(v[0]);
    check_contents(v, {1, 2, 3, 4, 1});

    Vector<int> w{1, 2, 3, 4};
    w.insert(0, w.back());
    check_contents(w, {4, 1, 2, 3, 4});

    Vector<std::string> s{"a", "b"};
    s.push_back(s[1]);
    CHECK_EQ(s[2], std::string("b"));
}


TEST(mod_erase, "[Modifiers] erase at front, middle, and end; keeps capacity", 3) {
    Vector<int> v = make_range(6);   // capacity 8
    v.erase(0);
    check_contents(v, {1, 2, 3, 4, 5});
    v.erase(2);
    check_contents(v, {1, 2, 4, 5});
    v.erase(v.size() - 1);
    check_contents(v, {1, 2, 4});
    CHECK_EQ(v.capacity(), 8);
    v.erase(0);
    v.erase(0);
    v.erase(0);
    CHECK(v.empty());
}


TEST(mod_erase_oob, "[Modifiers] erase out of range throws and leaves the vector unchanged", 2) {
    Vector<int> v{1, 2, 3};
    CHECK_THROWS(v.erase(3), std::out_of_range);
    CHECK_THROWS(v.erase(100), std::out_of_range);
    check_contents(v, {1, 2, 3});
    Vector<int> e;
    CHECK_THROWS(e.erase(0), std::out_of_range);
}


TEST(equality, "[Equality] operator== and operator!= compare elements", 2) {
    Vector<int> a{1, 2, 3};
    Vector<int> b = make_range(4);   // 0,1,2,3 with capacity 4
    b.erase(0);                      // 1,2,3
    CHECK(a == b);
    CHECK(!(a != b));
    b.reserve(50);                   // capacity doesn't matter
    CHECK(a == b);
    b[2] = 4;
    CHECK(a != b);
    CHECK(!(a == b));
    Vector<int> c{1, 2};
    CHECK(a != c);
    Vector<int> e1, e2;
    CHECK(e1 == e2);
    CHECK(e1 != a);
}


TEST(strings, "[Big 3] Vector<std::string> grows, copies, and erases correctly", 2) {
    Vector<std::string> v;
    for (int i = 0; i < 20; ++i)
        v.push_back("a fairly long string so it lives on the heap #" + std::to_string(i));
    Vector<std::string> w(v);
    v.erase(0);
    w.insert(0, "first");
    CHECK_EQ(v[0], std::string("a fairly long string so it lives on the heap #1"));
    CHECK_EQ(w[0], std::string("first"));
    CHECK_EQ(w[20], std::string("a fairly long string so it lives on the heap #19"));
}


TEST(stress, "[Stress] 20,000 random operations match std::vector, no leaks", 3) {
    alloc::Snapshot s;
    {
        std::mt19937 rng(32);
        Vector<int> v;
        std::vector<int> ref;
        for (int step = 0; step < 20000; ++step) {
            int op = rng() % 100;
            int val = static_cast<int>(rng() % 1000);
            if (op < 40) {
                v.push_back(val); ref.push_back(val);
            } else if (op < 55) {
                if (!ref.empty()) { v.pop_back(); ref.pop_back(); }
            } else if (op < 67) {
                std::size_t i = rng() % (ref.size() + 1);
                v.insert(i, val); ref.insert(ref.begin() + i, val);
            } else if (op < 79) {
                if (!ref.empty()) {
                    std::size_t i = rng() % ref.size();
                    v.erase(i); ref.erase(ref.begin() + i);
                }
            } else if (op < 85) {
                std::size_t n = rng() % (ref.size() + 20);
                v.resize(n); ref.resize(n);
            } else if (op < 89) {
                v.reserve(rng() % 300);
            } else if (op < 92) {
                v.shrink_to_fit();
                CHECK_EQ(v.capacity(), v.size());
            } else if (op < 96) {
                Vector<int> tmp(v);
                v = tmp;
            } else if (op < 97) {
                v.clear(); ref.clear();
            } else {
                Vector<int> tmp;
                tmp = v;
                v = Vector<int>(tmp);
            }
            CHECK(v.capacity() >= v.size());
            if (v.size() != ref.size()) {
                harness::fail(__FILE__, __LINE__, "size mismatch at step " + std::to_string(step)
                    + ": got " + std::to_string(v.size()) + ", expected " + std::to_string(ref.size()));
            }
            for (std::size_t i = 0; i < ref.size(); ++i) {
                if (v[i] != ref[i]) {
                    harness::fail(__FILE__, __LINE__, "element " + std::to_string(i)
                        + " wrong after step " + std::to_string(step));
                }
            }
        }
    }
    CHECK_EQ(s.live_delta(), 0);
}

int main(int argc, char** argv) { return harness_main(argc, argv); }
