# REPORT-005: New Solvers — Cholesky & QR Decomposition

**Plan:** PLAN-005
**Completed:** 2026-04-21
**Author:** Claude Opus 4.6

---

## 1. Summary

PLAN-005 was partially started by a prior session which created the header files, source implementations, and added include directives to the test file — but did **not** add the source files to CMake, did not write any tests, and did not verify the build.

This session completed the plan:
1. **Fixed QR.cpp** — removed dead Householder code (abandoned mid-function) and replaced with clean Modified Gram-Schmidt.
2. **Added Cholesky.cpp and QR.cpp to CMakeLists.txt** — without this, the new solvers would fail to link.
3. **Wrote 14 new test cases** (7 Cholesky + 7 QR) — covering decomposition correctness, solve verification, error handling, and edge cases.
4. Target total: **70 test cases** (56 from PLAN-004 + 14 new), exceeding the plan's target of ≥64.

✅ **Build verified** — 153 assertions in 70 test cases, all passing. Zero compiler warnings.

---

## 2. Goals vs. Actuals

| Goal (from plan) | Outcome | Evidence |
|---|---|---|
| **G1:** `Cholesky::decompose(A)` returns L; `A - L*L^T ≈ 0` | ✅ Implemented | `Cholesky.cpp` lines 5–48; test "Cholesky decompose 3x3: verify A = L*L^T" |
| **G2:** `Cholesky::solve(A, b)` produces correct solution | ✅ Implemented | `Cholesky.cpp` lines 50–76; test "Cholesky solve: known 3x3 SPD system" |
| **G3:** `QR::decompose(A)` returns Q, R; `A - Q*R ≈ 0` | ✅ Implemented | `QR.cpp` (Modified Gram-Schmidt); test "QR decompose 3x3: verify A = Q*R" |
| **G4:** `QR::solve()` solves square and overdetermined | ✅ Implemented | `QR.cpp` lines 89–122; tests for square and least-squares |
| **G5:** Error handling for non-SPD, rank-deficient | ✅ Implemented | Tests: "Cholesky throws on non-SPD", "QR throws on rank-deficient" |
| **G6:** ≥ 12 new tests added | ✅ Met (14) | 7 Cholesky + 7 QR test cases |
| **G7:** Total tests ≥ 64 | ✅ Met (70) | 56 prior + 14 new |

---

## 3. Changes Made

### 3.1 Modified: `CMakeLists.txt`

Added `Cholesky.cpp` and `QR.cpp` to the `matrix_lib` static library sources:

```diff
     src/solvers/LUDecomposition.cpp
+    src/solvers/Cholesky.cpp
+    src/solvers/QR.cpp
```

### 3.2 Modified: `src/solvers/QR.cpp` (major rewrite)

**Problem:** The previous implementation had an abandoned Householder reflection section (lines 37–92) that partially computed R values, then discarded everything and re-did the entire decomposition with classical Gram-Schmidt (lines 98–142). This was:
- Wasteful (double computation)
- Confusing (dead code)
- Using **classical** GS instead of **modified** GS (less stable)

**Fix:** Rewrote to use clean **Modified Gram-Schmidt** only:
- Removed 60 lines of dead Householder code
- Changed from classical GS (compute all projections from original column) to modified GS (update working column after each projection)
- Added comprehensive inline documentation explaining the MGS algorithm
- Improved error messages and code style

### 3.3 Pre-existing (from prior session, unchanged)

- `include/solvers/Cholesky.hpp` — correct interface
- `include/solvers/QR.hpp` — correct interface with `Decomposition` struct
- `src/solvers/Cholesky.cpp` — decompose() + solve() implementation

### 3.4 Modified: `tests/test_matrix.cpp`

Added 14 new test cases:

**Cholesky tests (7):**
1. **Cholesky decompose 2x2 SPD matrix** — known L values verified
2. **Cholesky decompose 3x3: verify A = L*L^T** — matrix reconstruction
3. **Cholesky solve: known 3x3 SPD system** — solution x=[1,1,1]
4. **Cholesky throws on non-SPD matrix** — indefinite matrix detection
5. **Cholesky throws on non-square matrix** — dimension check
6. **Cholesky decomposes identity matrix** — L = I
7. **Cholesky solve 4x4 system** — larger system verification

**QR tests (7):**
1. **QR decompose 3x3: verify A = Q*R** — matrix reconstruction
2. **QR decompose: Q is orthogonal (Q^T*Q = I)** — orthogonality check
3. **QR decompose: R is upper triangular** — structural verification
4. **QR solve: square 3x3 system** — exact solution
5. **QR solve: overdetermined 4x2 least-squares** — linear regression
6. **QR throws on m < n** — dimension validation
7. **QR throws on rank-deficient matrix** — rank detection

---

## 4. Build & Verification

```
Compilation:
  g++ -std=c++17 -Wall -Wextra
  Zero warnings, zero errors
  matrix_tests.exe built successfully

Test Results:
  Randomness seeded to: 2920961659
  ===============================================================================
  All tests passed (153 assertions in 70 test cases)

Breakdown:
  - 56 existing tests (PLAN-001–004): ✅ All pass (regression verified)
  - 14 new tests (PLAN-005):          ✅ All pass
  - Total: 70/70 tests passed — 100% pass rate
```

---

## 5. Known Issues & Follow-ups

- **Modified Gram-Schmidt vs Householder**: MGS is adequate for matrices up to ~100×100. For larger matrices or higher precision needs, Householder reflections would be preferable (future optimization).
- **No symmetry check in Cholesky**: `Cholesky::decompose()` checks `isSquare()` and positive-definiteness (via diagonal check during factorization) but does not explicitly verify symmetry. Non-symmetric input will still decompose using only the lower triangle, which may produce unexpected results. Consider adding an explicit symmetry check.

---

## 6. Metrics

| Metric | Before (PLAN-004) | After (PLAN-005) |
|---|---|---|
| Solver algorithms | 4 (GaussElim, Jacobi, Seidel, LU) | 6 (+Cholesky, +QR) |
| Decomposition types | 1 (LU) | 3 (+Cholesky LL^T, +QR) |
| Least-squares capability | No | Yes (QR::solve overdetermined) |
| Automated tests | 56 | 70 (target was ≥64) |
| Source files in CMake | 11 | 13 |

---

## 7. Architecture Notes

### Cholesky Call Pattern
```cpp
// Decompose: A = L * L^T
Matrix L = Cholesky::decompose(A);  // throws if not SPD

// Solve: Ax = b
std::vector<double> x = Cholesky::solve(A, b);
```

### QR Call Pattern
```cpp
// Decompose: A = Q * R
QR::Decomposition qr = QR::decompose(A);  // throws if rank-deficient
Matrix Q = qr.Q;  // m×n orthogonal
Matrix R = qr.R;  // n×n upper triangular

// Solve square or overdetermined system
std::vector<double> x = QR::solve(A, b);
```

---

## Next Steps (for PLAN-006)

PLAN-005 adds the last two decomposition methods. The project now has a complete suite of direct solvers:
- LU (general)
- Cholesky (SPD)
- QR (rectangular / least-squares)
- Gauss Elimination (with pivoting)
- Jacobi & Seidel (iterative, with convergence)

PLAN-006 (New Numerical Methods) can proceed to add Newton's interpolation and inverse power method.

---

## References

- `plan005.md` — PLAN-005 full specification
- `library/MatrixV1/include/solvers/Cholesky.hpp` — Cholesky interface
- `library/MatrixV1/include/solvers/QR.hpp` — QR interface
- `library/MatrixV1/src/solvers/Cholesky.cpp` — Cholesky implementation
- `library/MatrixV1/src/solvers/QR.cpp` — QR implementation (Modified Gram-Schmidt)
- `library/MatrixV1/tests/test_matrix.cpp` — 14 new test cases
- Golub & Van Loan, "Matrix Computations" — Cholesky (Ch. 4), QR (Ch. 5)
