# PLAN-002: Matrix Class Modernisation

**Created:** 2026-04-20
**Status:** Completed
**Addresses:** The `Matrix` class is a bare data container with no operator support, no safety checks, and no utility methods. Every algorithm in the library receives `const Matrix&` but cannot use natural C++ syntax — callers must go through static helper classes for every operation. This plan upgrades `Matrix` into a first-class numerical object.

---

## 1. Context & Motivation

After PLAN-001 the project has a clean CMake build and a working test suite. The `Matrix` class (`include/Matrix.hpp` / `src/Matrix.cpp`) currently provides only:

- A constructor, `getRows()`, `getCols()`
- Raw `get(i, j)` / `set(i, j, val)` with **no bounds checking**
- Console I/O (`input()`, `display()`) and file I/O (`inputFromFile()`, `saveToFile()`)

Everything else — addition, subtraction, multiplication — lives in separate static classes (`Add`, `Subtract`, `Multiply`, `Divide`). This is functional but means:

- No natural C++ syntax (`A + B` is `Add::compute(A, B)`)
- A silent out-of-bounds access on `get(-1, 0)` corrupts memory or crashes without a useful message
- No way to query `isSquare()` or `isSymmetric()` from inside solver code without writing it inline every time
- No `transpose()`, `determinant()`, or `inverse()` — three operations that nearly every subsequent feature depends on

This plan adds all of the above while keeping the existing static classes intact (operators delegate to them, so there is exactly one implementation per operation).

---

## 2. Goals

- **G1:** `Matrix` supports `operator+`, `operator-`, `operator*`, `operator==`, `operator!=` and `operator<<` (stream output). Operators delegate to the existing static classes (`Add`, `Subtract`, `Multiply`).
- **G2:** `get(i, j)` and `set(i, j, val)` throw `std::out_of_range` with a descriptive message when indices are out of bounds.
- **G3:** `Matrix` provides `transpose()` returning a new `Matrix`.
- **G4:** `Matrix` provides `determinant()` via two methods — LU-based (fast) and cofactor expansion (classic). User selects via an enum parameter; default is LU-based.
- **G5:** `Matrix` provides `inverse()` for any N×N invertible matrix, using Gauss-Jordan elimination. Throws `std::runtime_error` if the matrix is singular.
- **G6:** `Matrix` provides `isSquare()` and `isSymmetric()` predicate methods.
- **G7:** All new methods are covered by automated tests (`tests/test_matrix.cpp`); all existing `test_smoke.cpp` tests still pass.
- **G8:** `main.cpp` compiles and runs unchanged — no breaking changes to the existing public API.

---

## 3. Non-Goals

- Generalising `Divide::compute()` beyond 2×2 — that is PLAN-003 (it depends on `inverse()` from this plan).
- Iterative solver convergence fixes — PLAN-004.
- Scalar multiplication (`Matrix * double`) — can be added in a later plan if needed.
- Complex number support — deferred; not in current spec.
- In-place operators (`+=`, `-=`, `*=`) — deferred; not required by any current feature.
- Sparse matrix representation — out of scope for v1.

---

## 4. Approach

### 4.1 Operator overloading — delegating to static classes

Operators are implemented as **non-member friend functions** in `Matrix.hpp` and defined in `Matrix.cpp`. Each delegates directly to the corresponding static class:

```cpp
// In Matrix.hpp
friend Matrix operator+(const Matrix& A, const Matrix& B);
friend Matrix operator-(const Matrix& A, const Matrix& B);
friend Matrix operator*(const Matrix& A, const Matrix& B);
friend bool   operator==(const Matrix& A, const Matrix& B);
friend bool   operator!=(const Matrix& A, const Matrix& B);
friend std::ostream& operator<<(std::ostream& os, const Matrix& M);

// In Matrix.cpp
Matrix operator+(const Matrix& A, const Matrix& B) { return Add::compute(A, B); }
Matrix operator-(const Matrix& A, const Matrix& B) { return Subtract::compute(A, B); }
Matrix operator*(const Matrix& A, const Matrix& B) { return Multiply::compute(A, B); }
```

This means there is a single implementation per operation. Improving `Multiply::compute()` later (e.g. cache-optimised loop) automatically improves `operator*`.

`operator==` compares element-by-element within a tolerance of `1e-9` (floating point equality). `operator!=` returns `!operator==`.

`operator<<` calls the existing `display()` logic but writes to `std::ostream` instead of `std::cout` directly.

### 4.2 Bounds checking

`get()` and `set()` gain a bounds check before accessing `data[i][j]`:

```cpp
double Matrix::get(int i, int j) const {
    if (i < 0 || i >= rows || j < 0 || j >= cols)
        throw std::out_of_range("Matrix::get(" + std::to_string(i) + ", "
                                + std::to_string(j) + ") out of range ["
                                + std::to_string(rows) + "x"
                                + std::to_string(cols) + "]");
    return data[i][j];
}
```

Same pattern for `set()`. The error message includes both the attempted index and the actual dimensions — useful for debugging.

### 4.3 transpose()

Returns a new `Matrix` of dimensions `cols × rows`:

```cpp
Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.set(j, i, data[i][j]);
    return result;
}
```

### 4.4 determinant() — two methods, one interface

We add an enum to `Matrix.hpp`:

```cpp
enum class DetMethod { LU, Cofactor };
double determinant(DetMethod method = DetMethod::LU) const;
```

**LU-based (default):** Reuses `LU::decompose()`. The determinant equals the product of the diagonal of U, multiplied by the sign of the row permutation. Fast: O(n³).

**Cofactor expansion:** Classic recursive method. Expands along the first row. Correct for any size but O(n!) — suitable for small matrices and educational use.

Both methods throw `std::runtime_error` if the matrix is not square.

### 4.5 inverse() — Gauss-Jordan elimination

Uses augmented matrix `[A | I]` and applies full row reduction to produce `[I | A⁻¹]`. This is independent of the existing `LU::solve()` and is the standard textbook approach for computing the full inverse.

```cpp
Matrix Matrix::inverse() const;
```

Throws `std::runtime_error("Matrix is singular — inverse does not exist")` if a zero pivot is encountered. Throws `std::runtime_error("Matrix must be square")` if not square.

**Why not LU-based inverse?** Gauss-Jordan is simpler to implement correctly for the full inverse matrix (LU-based inverse requires N back-substitutions, one per identity column — more code for the same result).

### 4.6 isSquare() / isSymmetric()

```cpp
bool Matrix::isSquare()    const { return rows == cols; }
bool Matrix::isSymmetric() const;  // checks A[i][j] == A[j][i] within 1e-9
```

`isSymmetric()` returns `false` immediately if not square.

---

## 5. Task Breakdown

| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | Add `isSquare()`, `isSymmetric()` to `Matrix.hpp` / `Matrix.cpp` | 20 min | — |
| 2 | Write tests for `isSquare()` and `isSymmetric()` | 15 min | 1 |
| 3 | Add bounds checking to `get()` and `set()`; write tests | 20 min | — |
| 4 | Add `transpose()` to `Matrix.hpp` / `Matrix.cpp`; write tests | 20 min | — |
| 5 | Add `operator<<` (stream output); write tests | 15 min | — |
| 6 | Add `operator+`, `operator-`, `operator*` delegating to static classes; write tests | 30 min | — |
| 7 | Add `operator==`, `operator!=` with 1e-9 tolerance; write tests | 20 min | — |
| 8 | Add `DetMethod` enum and `determinant()` — LU-based path; write tests | 45 min | — |
| 9 | Add cofactor expansion path to `determinant()`; write tests | 30 min | 8 |
| 10 | Add `inverse()` via Gauss-Jordan; write tests | 45 min | 3 |
| 11 | Run full test suite — verify all new + existing tests pass | 15 min | 1–10 |
| 12 | Update `AGENT_MD/plan/current_state_report.md` | 10 min | 11 |

**Total estimated time: ~4 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| LU-based determinant gives wrong sign without tracking row swaps | Medium | Medium | Track pivot swap count during partial pivoting; multiply by `(-1)^swaps` |
| Gauss-Jordan inverse fails silently on near-singular matrices | Low | Medium | Use same `1e-12` pivot threshold as existing `GaussElimination`; throw on detection |
| `operator==` with floating-point tolerance causes unexpected test behaviour | Low | Low | Document tolerance (`1e-9`) in header comment; use `REQUIRE` with known values in tests |
| Bounds checking breaks existing code that passes wrong indices silently | Low | Low | This is intentional — surfacing silent bugs is a goal; no existing algorithm passes out-of-bounds indices |
| Adding `#include` for static classes in `Matrix.cpp` creates circular dependency | Low | Medium | Operators are defined in `Matrix.cpp` which already includes `Matrix.hpp`; adding `#include "operations/Add.hpp"` etc. is safe since those headers include `Matrix.hpp` with header guards |

---

## 7. Success Criteria

- [ ] **G1:** `Matrix C = A + B;` compiles and produces same result as `Add::compute(A, B)`
- [ ] **G1:** `std::cout << A;` prints the matrix without calling `A.display()`
- [ ] **G2:** `m.get(-1, 0)` throws `std::out_of_range` (verified by `REQUIRE_THROWS` test)
- [ ] **G3:** `A.transpose()` on a 2×3 matrix returns a 3×2 matrix with correctly swapped elements
- [ ] **G4:** `A.determinant()` and `A.determinant(DetMethod::Cofactor)` return the same value for a known 3×3 matrix
- [ ] **G4:** Both methods throw on a non-square matrix
- [ ] **G5:** `A.inverse() * A` equals the identity matrix within `1e-9` tolerance
- [ ] **G5:** `inverse()` throws on a singular matrix
- [ ] **G6:** `isSquare()` and `isSymmetric()` return correct results for known inputs
- [ ] **G7:** `ctest --test-dir build` exits 0, all tests pass (old + new)
- [ ] **G8:** `main.cpp` compiles without modification after all changes

---

## 8. References

- `AGENT_MD/spec.md` — Feature 1 (Matrix Class Modernisation)
- `AGENT_MD/plan/reports/REPORT-001_build_system_setup.md` — build baseline established
- `Matrix/include/Matrix.hpp` — file to be modified
- `Matrix/src/Matrix.cpp` — file to be modified
- `Matrix/include/operations/Add.hpp`, `Subtract.hpp`, `Multiply.hpp` — delegated to by operators
- `Matrix/src/solvers/LUDecomposition.cpp` — referenced by LU-based determinant path
- `Matrix/tests/test_smoke.cpp` — must remain passing (regression check)