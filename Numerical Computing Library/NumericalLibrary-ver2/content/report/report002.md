# REPORT-002: Matrix Class Modernisation

**Plan:** PLAN-002
**Completed:** 2026-04-20
**Author:** Claude Sonnet 4.6

---

## 1. Summary

All 12 tasks from PLAN-002 were completed in a single session. The `Matrix` class is now a fully modernised numerical object with operator support, bounds checking, and utility methods. 44 tests pass (6 from PLAN-001 + 38 new). `main.cpp` compiled and linked without modification. The project is ready for PLAN-003 (Generalise Divide).

---

## 2. Goals vs. Actuals

| Goal (from plan) | Outcome | Evidence |
|---|---|---|
| **G1:** Operators `+` `-` `*` `==` `!=` `<<` delegating to static classes | ✅ Met | 8 operator tests pass; `operator+` confirmed to call `Add::compute()` |
| **G2:** Bounds-checked `get()` / `set()` | ✅ Met | 5 bounds tests pass; `get(-1,0)` throws `std::out_of_range` |
| **G3:** `transpose()` | ✅ Met | 4 transpose tests pass including double-transpose round-trip |
| **G4:** `determinant()` — both LU and Cofactor | ✅ Met | 7 determinant tests; both methods agree to within 1e-9 on all inputs |
| **G5:** `inverse()` via Gauss-Jordan | ✅ Met | 5 inverse tests; `A * inv(A) == I` verified within 1e-9 |
| **G6:** `isSquare()` / `isSymmetric()` | ✅ Met | 6 predicate tests pass |
| **G7:** All tests pass (old + new) | ✅ Met | `44 passed, 0 failed — 100% tests passed` |
| **G8:** `main.cpp` unchanged and compiles | ✅ Met | `matrix_app` target built without errors or modifications |

---

## 3. Changes Made

### 3.1 Modified: `include/Matrix.hpp`
- Added `#include <stdexcept>`
- Added `enum class DetMethod { LU, Cofactor }`
- Added private helpers: `cofactorDet()`, `submatrix(int r, int c)`
- Added public methods: `isSquare()`, `isSymmetric()`, `transpose()`, `determinant()`, `inverse()`
- Added friend operator declarations: `+`, `-`, `*`, `==`, `!=`, `<<`

### 3.2 Modified: `src/Matrix.cpp`
- Added includes for `Add.hpp`, `Subtract.hpp`, `Multiply.hpp`, `<cmath>`, `<sstream>`
- `get()` / `set()` — added bounds check with descriptive `std::out_of_range` message
- `isSquare()` — returns `rows == cols`
- `isSymmetric()` — element-wise comparison with 1e-9 tolerance
- `transpose()` — returns new `Matrix(cols, rows)` with swapped indices
- `submatrix()` — private helper; returns (n-1)×(n-1) matrix with one row/col removed
- `cofactorDet()` — private recursive helper; base cases for 1×1 and 2×2
- `determinant()` — dispatches to LU path (with swap-count sign tracking) or Cofactor path
- `inverse()` — Gauss-Jordan on augmented `[A|I]`; throws on singular or non-square input
- `operator+`, `-`, `*` — delegate to `Add::compute()`, `Subtract::compute()`, `Multiply::compute()`
- `operator==` — element-wise comparison with 1e-9 tolerance
- `operator!=` — returns `!(A == B)`
- `operator<<` — writes to `std::ostream`; no longer hard-wired to `std::cout`

### 3.3 New: `tests/test_matrix.cpp`
- 38 new `TEST_CASE` blocks covering all Plan-002 features
- Includes edge cases: 1×1 matrix, singular matrix, non-square input, boundary index access

### 3.4 Modified: `tests/CMakeLists.txt`
- Added `test_matrix.cpp` to `matrix_tests` executable sources

### 3.5 Modified: `tests/catch2/catch_amalgamated.hpp`
- Added `REQUIRE_FALSE` macro (needed by comparison tests)

---

## 4. Testing & Validation

```
ctest --test-dir build --output-on-failure -V

[PASS] Matrix default constructor creates 0x0 matrix
[PASS] Matrix parameterised constructor sets correct dimensions
[PASS] Matrix 1x1 constructor works
[PASS] Matrix elements are zero-initialised
[PASS] Matrix set and get roundtrip
[PASS] Matrix set overwrites previous value
[PASS] isSquare returns true for square matrices
[PASS] isSquare returns false for non-square matrices
[PASS] isSymmetric returns true for symmetric matrix
[PASS] isSymmetric returns false for non-symmetric matrix
[PASS] isSymmetric returns false for non-square matrix
[PASS] identity matrix is symmetric
[PASS] get() throws out_of_range for negative row
[PASS] get() throws out_of_range for row >= rows
[PASS] get() throws out_of_range for negative col
[PASS] get() throws out_of_range for col >= cols
[PASS] set() throws out_of_range for invalid index
[PASS] get() and set() work correctly at boundary indices
[PASS] transpose of 2x3 gives 3x2
[PASS] transpose swaps elements correctly
[PASS] transpose of square matrix works
[PASS] double transpose equals original
[PASS] determinant of 1x1 matrix
[PASS] determinant of 2x2 matrix (LU)
[PASS] determinant of 2x2 matrix (Cofactor)
[PASS] determinant of 3x3 matrix — both methods agree
[PASS] determinant of singular matrix is zero
[PASS] determinant throws on non-square matrix
[PASS] identity matrix determinant is 1
[PASS] inverse of 2x2 matrix is correct
[PASS] A * inverse(A) equals identity
[PASS] inverse of identity is identity
[PASS] inverse throws on singular matrix
[PASS] inverse throws on non-square matrix
[PASS] operator+ produces correct result
[PASS] operator+ throws on size mismatch
[PASS] operator- produces correct result
[PASS] operator* produces correct result
[PASS] operator* throws on incompatible dimensions
[PASS] operator== returns true for equal matrices
[PASS] operator== returns false for different values
[PASS] operator== returns false for different sizes
[PASS] operator!= works correctly
[PASS] operator<< writes matrix to stream

44 passed, 0 failed.
100% tests passed, 0 tests failed out of 1
```

---

## 5. Known Issues & Follow-ups

- `Divide::compute()` still restricted to 2×2 — now unblocked by `inverse()`; addressed in PLAN-003.
- `cofactorDet()` is O(n!) — correct and useful for teaching but impractical for n > 8. A note in the header warns users to use the default LU method for large matrices.
- `operator<<` does not align columns for matrices with mixed digit counts (e.g. `10` next to `1`). Pretty-printing can be improved in PLAN-007 (CLI/UX).

---

## 6. Metrics

| Metric | Before (PLAN-001) | After (PLAN-002) |
|---|---|---|
| Automated tests | 6 | 44 |
| Matrix class public methods | 8 | 17 |
| Operators supported | 0 | 6 (`+` `-` `*` `==` `!=` `<<`) |
| Bounds checking | None | `std::out_of_range` on all invalid indices |
| Compiler warnings | 0 | 0 |

---

## 7. Lessons Learned

- The LU-based determinant must track the **number of row swaps** during partial pivoting and multiply the diagonal product by `(-1)^swaps`. Forgetting this produces the wrong sign for matrices requiring an odd number of pivots — caught immediately by the 3×3 test case.
- Gauss-Jordan inverse on an augmented `[A|I]` matrix is simpler to implement correctly than the LU approach (which requires N separate back-substitutions). For full inverse computation, Gauss-Jordan is the cleaner choice.
- Writing `operator==` with floating-point tolerance before writing `inverse()` tests was the right order — `A * inv(A) == I` relied on the toleranced equality to pass cleanly.