# PLAN-003: Generalise Divide (N×N)

**Created:** 2026-04-21
**Status:** Completed
**Addresses:** The `Divide` operation is hardcoded to 2×2 matrices only, preventing its use on larger matrices. With `Matrix::inverse()` added in PLAN-002, this restriction can be removed, enabling N×N matrix "division" via `A * inv(B)`.

---

## 1. Context & Motivation

After PLAN-002 completed the `Matrix` class modernization (including `inverse()`), the `Divide::compute()` method remains limited to 2×2 matrices. It manually computes the inverse using the determinant formula and then multiplies. This is functional for small matrices but unusable for N×N cases, which are common in numerical computing.

The operation computes `A * inv(B)` (matrix "division" by multiplying by the inverse), assuming B is square and invertible. Generalizing it unlocks full N×N support without changing the core logic — just replace the hardcoded inverse with `B.inverse()`.

This plan depends on PLAN-002's `inverse()` method and keeps the existing static class architecture intact.

---

## 2. Goals

- **G1:** `Divide::compute(A, B)` works for any N×N square invertible matrix B (where A has compatible dimensions for multiplication with inv(B)).
- **G2:** Remove the 2×2 restriction; throw appropriate errors for non-square or singular B.
- **G3:** All new functionality is covered by automated tests in `tests/test_matrix.cpp` or a new `tests/test_divide.cpp`; all existing tests still pass.
- **G4:** `main.cpp` compiles and runs unchanged — no breaking changes to the public API.
- **G5:** Performance remains acceptable for typical N (e.g., N ≤ 100); no regressions in correctness.

---

## 3. Non-Goals

- No changes to other operations (`Add`, `Subtract`, `Multiply`) or solvers.
- No new matrix operations (e.g., scalar division, element-wise division).
- No changes to `Matrix::inverse()` — it already handles N×N.
- No updates to iterative solvers or other features.
- No UI/CLI improvements in `main.cpp`.

---

## 4. Approach

### 4.1 Modify Divide::compute()

Update `src/operations/Divide.cpp` to:

- Remove the hardcoded 2×2 inverse calculation.
- Use `B.inverse()` to get the inverse matrix.
- Retain the check for B being square (via `B.isSquare()` or dimensions).
- Ensure `Multiply::compute(A, B.inverse())` handles dimension compatibility (A's columns must match B's rows, i.e., A.cols == B.rows since B is square).
- Throw `std::invalid_argument` if B is not square.
- Let `B.inverse()` throw `std::runtime_error` for singular matrices (as implemented in PLAN-002).

The logic remains: `return Multiply::compute(A, B.inverse());`

### 4.2 Testing

- Add tests in `tests/test_matrix.cpp` (or create `tests/test_divide.cpp` if needed) covering:
  - 2×2 case (regression test).
  - 3×3 invertible matrix.
  - Non-square B (should throw).
  - Singular B (should throw).
  - Dimension mismatch (A.cols != B.rows).
- Verify against known results (e.g., A * inv(B) * B ≈ A).

### 4.3 Validation

- Build and run tests: `cmake --build build && ctest --test-dir build`.
- Ensure `main.cpp` still compiles (check if it uses Divide; if so, test menu options).

---

## 5. Task Breakdown

| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | Review current `Divide.cpp` implementation and confirm dependencies | 10 min | — |
| 2 | Modify `Divide::compute()` to use `B.inverse()`; remove 2×2 hardcoded logic | 20 min | 1 |
| 3 | Add error checks for non-square B and dimension compatibility | 15 min | 2 |
| 4 | Write tests for N×N cases, edge cases (singular, non-square) | 30 min | 2 |
| 5 | Run full test suite — verify all tests pass, including regressions | 15 min | 4 |
| 6 | Build and test `main.cpp` — ensure no compilation errors | 10 min | 5 |
| 7 | Update `AGENT_MD/plan/current_state_report.md` with Divide generalization | 10 min | 6 |

**Total estimated time: ~1.5 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| `B.inverse()` throws unexpectedly for edge cases | Low | Medium | Rely on PLAN-002's tested `inverse()`; add specific tests for singular/non-square |
| Performance degradation for large N (Gauss-Jordan is O(N³)) | Low | Low | Document as acceptable for N ≤ 100; no changes needed unless profiling shows issues |
| Dimension mismatch not caught early | Low | Low | Add explicit check: `if (A.getCols() != B.getRows()) throw std::invalid_argument(...)` |
| Circular dependency or include issues with `Matrix.hpp` | Low | Low | `Divide.cpp` already includes `Matrix.hpp`; no changes to headers |
| Existing 2×2 usage in `main.cpp` breaks | Low | Low | Test compilation; if Divide is used in menu, verify manually |

---

## 7. Success Criteria

- [ ] **G1 verified:** `Divide::compute(A, B)` succeeds for 3×3+ matrices where B is invertible and dimensions match.
- [ ] **G2 verified:** Throws `std::invalid_argument` for non-square B; `std::runtime_error` for singular B (via `inverse()`).
- [ ] **G3 verified:** New tests added and passing; `ctest` shows 100% pass rate including existing tests.
- [ ] **G4 verified:** `matrix_app` builds and runs menu without errors.
- [ ] **G5 verified:** No performance regressions (manual check for N=10 matrix).

---

## 8. References

- `specs.md` — Feature 2 (Generalise Divide), Success Criteria v2.0
- `report002.md` — PLAN-002 completion, notes on Divide limitation
- `plan002.md` — Details on `Matrix::inverse()` implementation
- `library/MatrixV1/include/operations/Divide.hpp` — Current interface
- `library/MatrixV1/src/operations/Divide.cpp` — Current 2×2 implementation