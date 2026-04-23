# PLAN-006: New Numerical Methods — Newton Interpolation & Inverse Power Method

**Created:** 2026-04-22
**Status:** Completed
**Addresses:** Feature 6 from specs.md — extending the numerical methods module with Newton's divided difference interpolation and the inverse power method for the smallest eigenvalue.

---

## 1. Context & Motivation

After PLAN-005 added Cholesky and QR decomposition, the project has a complete direct solver stack. The focus now shifts back to the **numerical methods** module.

Currently the library has:
- `Interpolation::lagrange()` — Lagrange interpolation (functional, but only one method)
- `EigenPower::powerMethod()` — dominant eigenvalue (fixed 100 iterations, no convergence check)

This plan adds:
- `Interpolation::newton()` — Newton's divided differences (same output as Lagrange, but precomputes a coefficient table reusable for multiple query points)
- `EigenPower::inverseMethod()` — inverse power method (smallest eigenvalue, uses LU solve internally)

---

## 2. Goals

- **G1:** `Interpolation::newton(x, y, value)` returns interpolated value using Newton's divided differences. Result must match `lagrange()` on same data within 1e-9.
- **G2:** `EigenPower::inverseMethod(A, eigenvector, tol, maxIter, iters)` returns the smallest-magnitude eigenvalue. Convergence-based (tolerance + max iterations), consistent with PLAN-004 pattern.
- **G3:** Error handling: throw on empty/mismatched data (Newton); throw on non-square or singular matrix (inverse method).
- **G4:** ≥ 8 new test cases (≥ 4 Newton, ≥ 4 inverse power).
- **G5:** All 70 existing tests continue to pass (no regressions).
- **G6:** No changes to `main.cpp` — purely additive.

---

## 3. Approach

### 3.1 Newton's Divided Differences

**Algorithm:**
1. Build divided difference table from x, y data points.
2. Evaluate Newton polynomial at query `value` using nested (Horner-like) evaluation.

```
dd[i][0] = y[i]
dd[i][j] = (dd[i][j-1] - dd[i-1][j-1]) / (x[i] - x[i-j])

p(value) = dd[0][0]
         + dd[1][1] * (value - x[0])
         + dd[2][2] * (value - x[0]) * (value - x[1])
         + ...
```

**Key properties:**
- O(n²) table build, O(n) evaluation
- Identical mathematical result to Lagrange for same data
- Coefficient table reusable for multiple queries (efficient for batched evaluation)

**Interface addition to `Interpolation` class:**
```cpp
static double newton(const std::vector<double>& x,
                     const std::vector<double>& y,
                     double value);
```

### 3.2 Inverse Power Method

**Algorithm:** Find eigenvector of smallest magnitude eigenvalue:
1. Start with initial guess `v = [1, 1, ..., 1]`
2. Each iteration: solve `A * w = v` (LU solve), normalize `w → v`
3. Eigenvalue estimate: `lambda = v^T * A * v` (Rayleigh quotient) or `1 / ||w||`
4. Converge when `||v_new - v_old||_2 < tol`

```
v = [1, ..., 1] / ||[1, ..., 1]||
for k = 1 to maxIter:
    w = LU::solve(A, v)          // A * w = v
    lambda_inv = max_norm(w)     // ||w||_inf
    v_new = w / lambda_inv
    if ||v_new - v_old||_2 < tol:
        break
    v = v_new
eigenvalue = 1 / lambda_inv      // smallest eigenvalue
```

**Interface addition to `EigenPower` class:**
```cpp
static double inverseMethod(const Matrix& A,
                            std::vector<double>& eigenvector,
                            double tol = 1e-9,
                            int maxIter = 1000,
                            int& iters = /* internal */);
```

Since `iters` output is optional, we use an overload pattern consistent with PLAN-004:
```cpp
// Overload 1: simple (no iteration count output)
static double inverseMethod(const Matrix& A,
                            std::vector<double>& eigenvector,
                            double tol = 1e-9,
                            int maxIter = 1000);

// Overload 2: with iteration count
static double inverseMethod(const Matrix& A,
                            std::vector<double>& eigenvector,
                            double tol, int maxIter, int& iters);
```

---

## 4. Files Changed

| File | Change |
|------|--------|
| `include/numerical/Interpolation.hpp` | Add `newton()` declaration |
| `src/numerical/Interpolation.cpp` | Add `newton()` implementation |
| `include/numerical/EigenPower.hpp` | Add `inverseMethod()` declarations (2 overloads) |
| `src/numerical/EigenPower.cpp` | Add `inverseMethod()` implementation |
| `tests/test_matrix.cpp` | Add ≥ 8 new test cases |

No CMakeLists.txt changes needed (files already registered).

---

## 5. Success Criteria

- [ ] `newton()` matches `lagrange()` on same data within 1e-9
- [ ] `inverseMethod()` returns smallest eigenvalue for known test matrix
- [ ] Both throw correctly on invalid input
- [ ] All 70 prior tests still pass
- [ ] Total test count ≥ 78

---

## 6. References

- `specs.md` — Feature 6 (New Numerical Methods)
- `library/MatrixV1/src/numerical/Interpolation.cpp` — existing Lagrange pattern
- `library/MatrixV1/src/numerical/EigenPower.cpp` — existing power method pattern
- `library/MatrixV1/src/solvers/LUDecomposition.cpp` — LU solve (used by inverse method)
