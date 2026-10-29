// Test harness for P01 Part 1. You do not need to modify this file, but
// reading it will help you understand what the tests check.
//
// It provides:
//   1. Allocation counting: global operator new/delete are replaced so the
//      tests can see every heap allocation your Vector makes.
//   2. Tracker: an element type that counts its constructions/destructions.
//   3. A tiny test framework (TEST, CHECK, CHECK_EQ, CHECK_THROWS).
//
// This header must be included by exactly one .cpp file (tests.cpp).

#ifndef HARNESS_H
#define HARNESS_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>
#include <sstream>
#include <string>
#include <type_traits>

// ============================================================================
// 1. Allocation counting
// ============================================================================

namespace alloc {
    inline long long news = 0;       // successful calls to operator new / new[]
    inline long long deletes = 0;    // calls to operator delete / delete[] with non-null ptr
    constexpr int LOG_SIZE = 64;
    inline std::size_t log[LOG_SIZE]; // requested byte sizes of recent allocations
    inline int log_count = 0;

    inline long long live() { return news - deletes; }

    // Snapshot of the counters; take one before an operation, then ask how
    // many allocations/deallocations happened since.
    struct Snapshot {
        long long n, d;
        int log_start;
        Snapshot() : n(news), d(deletes), log_start(log_count) {}
        long long allocs() const { return news - n; }
        long long frees() const { return deletes - d; }
        long long live_delta() const { return (news - n) - (deletes - d); }
        // byte size of the k-th allocation made since this snapshot
        std::size_t alloc_bytes(int k) const { return log[(log_start + k) % LOG_SIZE]; }
    };

    inline void* do_alloc(std::size_t n) {
        void* p = std::malloc(n ? n : 1);
        if (!p) throw std::bad_alloc();
        log[log_count % LOG_SIZE] = n;
        ++log_count;
        ++news;
        return p;
    }
    inline void do_free(void* p) {
        if (!p) return;
        ++deletes;
        std::free(p);
    }
}

void* operator new(std::size_t n) { return alloc::do_alloc(n); }
void* operator new[](std::size_t n) { return alloc::do_alloc(n); }
void operator delete(void* p) noexcept { alloc::do_free(p); }
void operator delete[](void* p) noexcept { alloc::do_free(p); }
void operator delete(void* p, std::size_t) noexcept { alloc::do_free(p); }
void operator delete[](void* p, std::size_t) noexcept { alloc::do_free(p); }

// ============================================================================
// 2. Tracker: counts what a container does to its elements
// ============================================================================

struct Tracker {
    inline static int default_ctors = 0;
    inline static int value_ctors = 0;
    inline static int copy_ctors = 0;
    inline static int copy_assigns = 0;
    inline static int dtors = 0;
    inline static int alive = 0;   // objects currently in existence

    int value;

    Tracker() : value(0) { ++default_ctors; ++alive; }
    Tracker(int v) : value(v) { ++value_ctors; ++alive; }
    Tracker(const Tracker& o) : value(o.value) { ++copy_ctors; ++alive; }
    Tracker& operator=(const Tracker& o) { value = o.value; ++copy_assigns; return *this; }
    ~Tracker() { ++dtors; --alive; }

    bool operator==(const Tracker& o) const { return value == o.value; }
    bool operator!=(const Tracker& o) const { return value != o.value; }

    static int copies() { return copy_ctors + copy_assigns; }
    static void reset() {
        default_ctors = value_ctors = copy_ctors = copy_assigns = dtors = alive = 0;
    }
};

// ============================================================================
// 3. Mini test framework
// ============================================================================

namespace harness {
    using TestFn = void (*)();
    struct TestCase { const char* name; int points; TestFn fn; };

    constexpr int MAX_TESTS = 256;
    inline TestCase tests[MAX_TESTS];
    inline int num_tests = 0;

    struct Registrar {
        Registrar(const char* name, int points, TestFn fn) {
            tests[num_tests++] = TestCase{name, points, fn};
        }
    };

    struct Failure { std::string msg; };

    [[noreturn]] inline void fail(const char* file, int line, const std::string& msg) {
        std::ostringstream os;
        os << file << ":" << line << ": " << msg;
        throw Failure{os.str()};
    }

    // Compare integers of mixed signedness without warnings or surprises.
    template <typename A, typename B>
    bool equal(const A& a, const B& b) {
        if constexpr (std::is_integral_v<A> && std::is_integral_v<B>) {
            return static_cast<long long>(a) == static_cast<long long>(b);
        } else {
            return a == b;
        }
    }

    template <typename X>
    std::string show(const X& x) {
        if constexpr (std::is_same_v<X, Tracker>) {
            return "Tracker(" + std::to_string(x.value) + ")";
        } else if constexpr (std::is_same_v<X, bool>) {
            return x ? "true" : "false";
        } else if constexpr (std::is_pointer_v<X> || std::is_null_pointer_v<X>) {
            if (x == nullptr) return "nullptr";
            std::ostringstream os; os << static_cast<const void*>(x); return os.str();
        } else {
            std::ostringstream os; os << x; return os.str();
        }
    }
}

#define TEST(fn_name, display_name, points)                                   \
    static void fn_name();                                                    \
    static harness::Registrar fn_name##_registrar(display_name, points, fn_name); \
    static void fn_name()

#define CHECK(cond)                                                           \
    do {                                                                      \
        if (!(cond)) harness::fail(__FILE__, __LINE__, "CHECK(" #cond ") failed"); \
    } while (0)

#define CHECK_EQ(actual, expected)                                            \
    do {                                                                      \
        auto&& _a = (actual);                                                 \
        auto&& _e = (expected);                                               \
        if (!harness::equal(_a, _e))                                          \
            harness::fail(__FILE__, __LINE__,                                 \
                "CHECK_EQ(" #actual ", " #expected ") failed\n    got:      " \
                + harness::show(_a) + "\n    expected: " + harness::show(_e)); \
    } while (0)

#define CHECK_THROWS(expr, ExType)                                            \
    do {                                                                      \
        bool _threw = false;                                                  \
        try { (void)(expr); }                                                 \
        catch (const ExType&) { _threw = true; }                              \
        catch (const harness::Failure&) { throw; }                            \
        catch (const std::exception& _ex) {                                   \
            harness::fail(__FILE__, __LINE__, "expected " #expr " to throw "  \
                #ExType ", but it threw a different exception: " + std::string(_ex.what())); \
        }                                                                     \
        if (!_threw)                                                          \
            harness::fail(__FILE__, __LINE__, "expected " #expr " to throw " #ExType); \
    } while (0)

// Runs a single test by ID (its position in tests.cpp, starting at 1), or
// lists all tests with --list.
inline int harness_main(int argc, char** argv) {
    using namespace harness;
    if (argc == 2 && std::strcmp(argv[1], "--list") == 0) {
        std::printf("%3s  %6s  %s\n", "ID", "Points", "Test");
        for (int i = 0; i < num_tests; ++i)
            std::printf("%3d  %6d  %s\n", i + 1, tests[i].points, tests[i].name);
        return 0;
    }
    char* rest = nullptr;
    long id = argc == 2 ? std::strtol(argv[1], &rest, 10) : 0;
    if (argc != 2 || rest == argv[1] || *rest != '\0') {
        std::fprintf(stderr,
            "usage: %s --list    list all tests and their IDs\n"
            "       %s <ID>      run one test, e.g. %s 12\n"
            "To run the whole suite, use: make test\n", argv[0], argv[0], argv[0]);
        return 2;
    }
    if (id < 1 || id > num_tests) {
        std::fprintf(stderr, "no test with ID %ld (IDs are 1 to %d; try --list)\n", id, num_tests);
        return 2;
    }
    const TestCase& test = tests[id - 1];
    Tracker::reset();
    try {
        test.fn();
    } catch (const Failure& f) {
        std::printf("FAIL: %s\n", f.msg.c_str());
        return 1;
    } catch (const std::exception& e) {
        std::printf("FAIL: uncaught exception: %s\n", e.what());
        return 1;
    } catch (...) {
        std::printf("FAIL: uncaught non-standard exception\n");
        return 1;
    }
    std::printf("PASS\n");
    return 0;
}

#endif
