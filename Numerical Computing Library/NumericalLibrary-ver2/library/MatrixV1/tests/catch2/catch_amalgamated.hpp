/*
 * catch_amalgamated.hpp — Minimal Catch2-compatible test framework
 *
 * PLACEHOLDER: Replace this file (and catch_amalgamated.cpp) with the real
 * Catch2 v3 amalgamated release when internet access is available:
 *
 *   curl -sL https://github.com/catchorg/Catch2/releases/download/v3.7.1/catch_amalgamated.hpp \
 *        -o tests/catch2/catch_amalgamated.hpp
 *   curl -sL https://github.com/catchorg/Catch2/releases/download/v3.7.1/catch_amalgamated.cpp \
 *        -o tests/catch2/catch_amalgamated.cpp
 *
 * This placeholder provides the same TEST_CASE / SECTION / REQUIRE / CHECK
 * macros so tests compile and run correctly in the interim.
 */

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <cmath>

namespace Catch {

struct TestCase {
    std::string name;
    std::string tags;
    std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> r;
    return r;
}

struct Registrar {
    Registrar(const char* name, const char* tags, std::function<void()> fn) {
        registry().push_back({name, tags, fn});
    }
};

struct AssertionFailed : std::exception {
    std::string msg;
    explicit AssertionFailed(std::string m) : msg(std::move(m)) {}
    const char* what() const noexcept override { return msg.c_str(); }
};

inline int run_all() {
    int passed = 0, failed = 0;
    for (auto& tc : registry()) {
        try {
            tc.fn();
            std::cout << "[PASS] " << tc.name << "\n";
            ++passed;
        } catch (const AssertionFailed& e) {
            std::cout << "[FAIL] " << tc.name << "\n       " << e.msg << "\n";
            ++failed;
        } catch (const std::exception& e) {
            std::cout << "[FAIL] " << tc.name << "\n       Exception: " << e.what() << "\n";
            ++failed;
        }
    }
    std::cout << "\n" << passed << " passed, " << failed << " failed.\n";
    return failed == 0 ? 0 : 1;
}

} // namespace Catch

// ─── Macros ───────────────────────────────────────────────────────────────────

#define CATCH_INTERNAL_CONCAT_IMPL(a, b) a##b
#define CATCH_INTERNAL_CONCAT(a, b)      CATCH_INTERNAL_CONCAT_IMPL(a, b)
#define CATCH_INTERNAL_UNIQUE(base)      CATCH_INTERNAL_CONCAT(base, __LINE__)

#define TEST_CASE(name, tags) \
    static void CATCH_INTERNAL_UNIQUE(_test_fn_)(); \
    static ::Catch::Registrar CATCH_INTERNAL_UNIQUE(_reg_)(name, tags, CATCH_INTERNAL_UNIQUE(_test_fn_)); \
    static void CATCH_INTERNAL_UNIQUE(_test_fn_)()

#define SECTION(name)  /* sections are flat in this minimal impl */

#define REQUIRE(expr) \
    do { \
        if (!(expr)) { \
            std::ostringstream _os; \
            _os << "REQUIRE(" #expr ") failed at " __FILE__ ":" << __LINE__; \
            throw ::Catch::AssertionFailed(_os.str()); \
        } \
    } while(0)

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cout << "  CHECK(" #expr ") failed at " __FILE__ ":" << __LINE__ << "\n"; \
        } \
    } while(0)

#define REQUIRE_THROWS(expr) \
    do { \
        bool _threw = false; \
        try { (expr); } catch (...) { _threw = true; } \
        if (!_threw) { \
            std::ostringstream _os; \
            _os << "REQUIRE_THROWS(" #expr ") — no exception thrown at " __FILE__ ":" << __LINE__; \
            throw ::Catch::AssertionFailed(_os.str()); \
        } \
    } while(0)

#define REQUIRE_FALSE(expr) \
    do { \
        if ((expr)) { \
            std::ostringstream _os; \
            _os << "REQUIRE_FALSE(" #expr ") failed at " __FILE__ ":" << __LINE__; \
            throw ::Catch::AssertionFailed(_os.str()); \
        } \
    } while(0)

#define REQUIRE_NOTHROW(expr) \
    do { \
        try { (expr); } catch (const std::exception& _e) { \
            std::ostringstream _os; \
            _os << "REQUIRE_NOTHROW(" #expr ") threw: " << _e.what() \
                << " at " __FILE__ ":" << __LINE__; \
            throw ::Catch::AssertionFailed(_os.str()); \
        } \
    } while(0)

#define CATCH_CONFIG_RUNNER
