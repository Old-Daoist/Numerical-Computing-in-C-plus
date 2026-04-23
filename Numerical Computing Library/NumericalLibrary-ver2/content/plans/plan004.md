# PLAN-004: Iterative Solver Convergence

**Created:** 2026-04-21
**Status:** Completed
**Addresses:** The `GaussJacobi` and `GaussSeidel` iterative solvers currently use a fixed iteration count with no convergence checking. This limits their practical use — they may over-iterate on well-conditioned systems (wasting computation) or under-iterate on ill-conditioned ones. This plan adds tolerance-based convergence detection and iteration reporting.

---

## 1. Context & Motivation

After PLAN-003 completed the generalization of `Divide`, the project's matrix operations are now complete. The focus shifts to improving the quality and usability of iterative solvers.

Currently (from [src/solvers/GaussJacobi.cpp](src/solvers/GaussJacobi.cpp) and [src/solvers/GaussSeidel.cpp](src/solvers/GaussSeidel.cpp)):
- Both solvers run for a **fixed number of iterations** (e.g., 100 or 1000).
- No convergence check — they iterate regardless of whether the solution has stabilized.
- No way to know how many iterations were actually needed.
- Users must guess the iteration count upfront.

This is inefficient and non-standard. Textbook iterative solvers all accept:
- **Tolerance** (`tol`, typically 1e-6 to 1e-9) — stop when `||x_k - x_{k-1}|| < tol`
- **Max iterations** (`maxIter`) — prevent infinite loops if convergence is slow/impossible
- **Iteration count** (output) — report how many iterations were performed

This plan adds all three to both Jacobi and Seidel.

---

## 2. Goals

- **G1:** `GaussJacobi::solve()` and `GaussSeidel::solve()` accept parameters: `double tol` (convergence tolerance) and `int maxIter` (maximum iterations).
- **G2:** Both solvers check convergence at each iteration: stop if `||x_k - x_{k-1}||_2 < tol` or `k >= maxIter`.
- **G3:** Both solvers return the number of iterations performed (via output parameter or modified return signature).
- **G4:** Default parameters are reasonable: `tol = 1e-6`, `maxIter = 1000`.
- **G5:** All existing `main.cpp` calls continue to work (backward compatible).
- **G6:** New tests in `tests/test_matrix.cpp` verify convergence detection and iteration counting; all existing tests still pass.
- **G7:** Documentation (inline comments) explains the convergence criterion and numerical considerations.

---

## 3. Non-Goals

- No changes to `GaussElimination` (direct solver) — it has no iterations.
- No changes to other algorithms (`Add`, `Multiply`, `Determinant`, etc.).
- No new iterative methods (GMRES, BiCG, etc.) — deferred to later plans.
- No adaptive step size or preconditioning — beyond scope.
- No visualization of convergence curves.

---

## 4. Approach

### 4.1 Convergence Detection

**L2-norm convergence criterion:**
$$\|\mathbf{x}^{(k)} - \mathbf{x}^{(k-1)}\|_2 < \text{tol}$$

Where $\|\mathbf{v}\|_2 = \sqrt{\sum_i v_i^2}$ is the Euclidean norm.

**Pseudocode:**
```
x = initial guess (or zero vector)
for k = 1 to maxIter:
    x_new = compute_iteration(x)
    delta = ||x_new - x||_2
    if delta < tol:
        return (x_new, k)  // converged
    x = x_new
return (x, maxIter)  // reached max iterations
```

### 4.2 Signature Modifications

#### GaussJacobi

**Before:**
```cpp
static Matrix solve(const Matrix& A, const Matrix& b);
```

**After:**
```cpp
static Matrix solve(const Matrix& A, const Matrix& b, 
                    double tol = 1e-6, int maxIter = 1000);

// Overload that also returns iteration count:
static Matrix solve(const Matrix& A, const Matrix& b, 
                    double tol, int maxIter, int& iters);
```

The second overload allows callers to retrieve `iters` (number of iterations performed).

#### GaussSeidel

Same approach as Jacobi.

### 4.3 Implementation Details

**Convergence check (compute L2 norm):**
```cpp
double delta = 0.0;
for (int i = 0; i < n; i++) {
    double diff = x_new.get(i, 0) - x.get(i, 0);
    delta += diff * diff;
}
delta = std::sqrt(delta);
```

**Early termination:**
```cpp
if (delta < tol) {
    iters = k;
    return x_new;
}
```

**Default parameters:** `tol = 1e-6` (standard for engineering), `maxIter = 1000` (balance between coverage and runtime).

### 4.4 Testing Strategy

**Existing test cases:** Retest with known solutions (from `test_cases/` `.txt` files) to ensure correctness is preserved.

**New test cases:**
- **Converged within tolerance** — well-conditioned system, verify `iters < maxIter`.
- **Hit max iterations** — ill-conditioned or nearly-singular system, verify `iters == maxIter` and warning/note in output.
- **Different tolerances** — tighter `tol` → more iterations; looser `tol` → fewer iterations.
- **Iteration count output** — verify `iters` parameter is set correctly.
- **Jacobi vs Seidel convergence speed** — Seidel typically converges faster; document if so.

### 4.5 Backward Compatibility

Both function signatures are **overloads**:
- Existing calls like `GaussJacobi::solve(A, b)` will use defaults (`tol=1e-6`, `maxIter=1000`).
- New calls can pass `tol` and `maxIter` explicitly.
- `main.cpp` will continue to work without modification — users see no change unless they opt in to controlling convergence.

---

## 5. Task Breakdown

| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | Review current `GaussJacobi.cpp` and `GaussSeidel.cpp` implementations | 15 min | — |
| 2 | Add convergence check logic (L2-norm computation) to Jacobi | 25 min | 1 |
| 3 | Add overload signatures for Jacobi with `tol`, `maxIter`, `iters` output | 20 min | 2 |
| 4 | Repeat steps 2–3 for Gauss-Seidel | 25 min | 1 |
| 5 | Update `SolveLinearEquation.hpp` (interface file) with new signatures | 15 min | 3, 4 |
| 6 | Write tests for convergence detection, iteration counting | 40 min | 5 |
| 7 | Run full test suite — verify all tests pass (no regressions) | 15 min | 6 |
| 8 | Manual test: run `main.cpp` menu; verify Jacobi/Seidel options still work | 10 min | 7 |
| 9 | Update `AGENT_MD/plan/current_state_report.md` with solver improvements | 10 min | 8 |

**Total estimated time: ~2.5 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Convergence criterion is too strict (tol too small by default) | Low | Medium | Use industry-standard `1e-6`; document in header; allow override |
| L2-norm computation inefficient for large N | Low | Low | Cache previous x; sqrt is O(n) anyway — acceptable |
| Overloading by `iters` output parameter may confuse callers | Low | Low | Provide clear documentation; use default parameters; example in comments |
| Ill-conditioned systems never converge, always hit maxIter | Low | Medium | Document that `maxIter` prevents infinite loops; recommend `tol` adjustment for pathological cases |
| main.cpp menu doesn't report iteration count | Low | Low | Optional enhancement: print `iters` when displaying solution (future UX improvement) |

---

## 7. Success Criteria

- [ ] **G1 verified:** Both solvers accept `tol` and `maxIter` parameters with sensible defaults.
- [ ] **G2 verified:** Convergence detected correctly; `ctest` shows passing test for early termination.
- [ ] **G3 verified:** `iters` output parameter (or return type) correctly reflects iterations performed.
- [ ] **G4 verified:** Defaults (`tol=1e-6`, `maxIter=1000`) used when parameters omitted.
- [ ] **G5 verified:** `main.cpp` compiles and runs unchanged.
- [ ] **G6 verified:** ≥ 5 new tests added; all 49+ tests pass.
- [ ] **G7 verified:** Inline documentation explains convergence criterion and numerical stability considerations.

---

## 8. References

- `specs.md` — Feature 3 (Iterative Solver Convergence), Success Criteria v2.0
- `library/MatrixV1/src/solvers/GaussJacobi.cpp` — Current fixed-iteration implementation
- `library/MatrixV1/src/solvers/GaussSeidel.cpp` — Current fixed-iteration implementation
- `library/MatrixV1/include/solvers/SolveLinearEquation.hpp` — Interface file
- `test_cases/matrix3.txt` — Diagonally dominant system (good for Jacobi/Seidel)
- Numerical Analysis textbooks — standard convergence criteria for iterative methods