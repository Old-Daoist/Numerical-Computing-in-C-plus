# PLAN-005: New Solvers — Cholesky & QR Decomposition

**Created:** 2026-04-21
**Status:** Completed
**Addresses:** The library currently has LU decomposition for general matrices but lacks specialized decompositions for symmetric positive-definite (SPD) and rectangular matrices. This plan adds Cholesky (for SPD systems) and QR (for least-squares and rectangular systems), two fundamental decompositions in numerical computing.

---

## 1. Context & Motivation

After PLAN-004 completed the iterative solver convergence improvements, the project has a solid foundation of matrix operations and direct/iterative solvers. The focus now shifts to adding **specialized decomposition methods** that are essential for real-world problems:

### Why Cholesky?
- **SPD matrices** (e.g., covariance matrices, finite element stiffness matrices) are common in engineering and statistics.
- Cholesky is **2× faster** than LU for SPD matrices (only needs lower triangle).
- **Numerically stable** — doesn't require pivoting.
- Solves `Ax = b` via: `A = LL^T` → solve `Ly = b` → solve `L^T x = y`.

### Why QR?
- Handles **rectangular matrices** (m×n with m ≥ n) for least-squares problems.
- Solves **overdetermined systems** (more equations than unknowns).
- **Numerically stable** — preferred over normal equations for least-squares.
- Applications: linear regression, curve fitting, data analysis.

---

## 2. Goals

- **G1:** Implement `Cholesky::decompose(A)` for symmetric positive-definite matrices. Returns L (lower triangular) where A = LL^T.
- **G2:** Implement `Cholesky::solve(A, b)` using the decomposition. Returns x satisfying Ax = b.
- **G3:** Implement `QR::decompose(A)` for m×n matrices (m ≥ n). Returns Q (m×n orthogonal) and R (n×n upper triangular) where A = QR.
- **G4:** Implement `QR::solve(A, b)` for rectangular systems. Uses QR decomposition; for overdetermined systems, solves least-squares.
- **G5:** Error handling: throw exceptions for non-SPD input to Cholesky, rank-deficient matrices to QR (if detected).
- **G6:** Comprehensive tests (≥ 8 new test cases) covering well-conditioned, ill-conditioned, and edge-case systems.
- **G7:** All existing tests pass (no regressions); total test count ≥ 64.

---

## 3. Non-Goals

- No changes to existing solvers (LU, Gauss Elimination, Jacobi, Seidel).
- No eigenvalue computation via QR iteration (that is a later feature).
- No pivoting strategies beyond what is necessary for stability.
- No parallelization or SIMD optimization.
- No LAPACK/BLAS integration (pure C++ STL only).

---

## 4. Approach

### 4.1 Cholesky Decomposition

**Doolittle algorithm (standard approach):**

For SPD matrix A (n×n), compute L (lower triangular) such that A = LL^T:

```
for i = 0 to n-1:
    for j = 0 to i:
        if j == i:
            sum = A[i][i]
            for k = 0 to i-1:
                sum -= L[i][k] * L[i][k]
            if sum <= 0:
                throw RuntimeError("Matrix not SPD")
            L[i][i] = sqrt(sum)
        else:
            sum = A[i][j]
            for k = 0 to j-1:
                sum -= L[i][k] * L[j][k]
            L[i][j] = sum / L[j][j]
```

**Key properties:**
- Requires O(n³/3) multiplications (half of LU).
- Only accesses lower triangle of A (symmetric).
- Detects non-SPD immediately (negative under sqrt).

**File structure:**
```cpp
// include/solvers/Cholesky.hpp
class Cholesky {
public:
    static Matrix decompose(const Matrix& A);  // returns L
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b);
};
```

### 4.2 QR Decomposition

**Householder reflection method (numerically stable):**

For m×n matrix A (m ≥ n), compute orthogonal Q (m×n) and upper triangular R (n×n):

```
for j = 0 to n-1:
    // Compute Householder vector for column j (from row j onwards)
    x = A[j:m, j]  (subvector)
    sigma = ||x||_2
    if sigma < 1e-12:
        throw RuntimeError("Matrix rank-deficient")
    v = x + sign(x[0]) * sigma * e_1
    v = v / ||v||_2
    
    // Apply Householder transformation to remaining columns
    for k = j to n-1:
        y = A[j:m, k]
        A[j:m, k] -= 2 * (v^T * y) * v
```

**Then extract:**
- Q = accumulated Householder reflections
- R = upper triangle of transformed A

**File structure:**
```cpp
// include/solvers/QR.hpp
class QR {
public:
    struct Decomposition {
        Matrix Q;  // m×n orthogonal
        Matrix R;  // n×n upper triangular
    };
    
    static Decomposition decompose(const Matrix& A);
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b);
};
```

For solve:
- **Square system (m=n):** `x = R^{-1} * Q^T * b` (via back-substitution)
- **Overdetermined (m>n):** Least-squares: `x = R^{-1} * Q^T * b` (same formula, solves in LS sense)

### 4.3 Testing Strategy

**Cholesky tests:**
1. SPD 2×2 matrix (known decomposition)
2. SPD 3×3 matrix (compute L, verify A = LL^T)
3. Near-singular SPD matrix (convergence check with tolerance)
4. Non-SPD matrix detection (throw exception)
5. Cholesky solve: known solution verification
6. Condition number comparison (Cholesky vs LU on same SPD system)

**QR tests:**
1. Square invertible matrix (m=n)
2. Overdetermined system (m>n) — least-squares solve
3. QR decomposition accuracy (verify A = QR, Q^T*Q ≈ I)
4. Rank-deficient detection (if applicable)
5. Least-squares residual (for overdetermined system)
6. Backward compatibility — solve square systems same as LU

---

## 5. Task Breakdown

| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | Design and write `include/solvers/Cholesky.hpp` interface | 15 min | — |
| 2 | Implement `Cholesky::decompose()` in `src/solvers/Cholesky.cpp` | 30 min | 1 |
| 3 | Implement `Cholesky::solve()` using forward/back substitution | 25 min | 2 |
| 4 | Write tests for Cholesky (6+ test cases) | 30 min | 3 |
| 5 | Design and write `include/solvers/QR.hpp` interface | 15 min | — |
| 6 | Implement `QR::decompose()` using Householder reflections | 45 min | 5 |
| 7 | Implement `QR::solve()` for square and overdetermined systems | 30 min | 6 |
| 8 | Write tests for QR (6+ test cases) | 35 min | 7 |
| 9 | Run full test suite — verify all tests pass (≥ 64 total) | 15 min | 4, 8 |
| 10 | Update `AGENT_MD/plan/current_state_report.md` with new solvers | 10 min | 9 |

**Total estimated time: ~3.5 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Cholesky fails to detect non-SPD due to numerical precision | Medium | Medium | Use tolerance check on diagonal: `if (diag_elem < 1e-12) throw` instead of `== 0` |
| QR Householder computation unstable for near-zero columns | Medium | Medium | Rank detection: skip columns with `||x|| < 1e-12`; or use modified Gram-Schmidt as alternative |
| QR decomposition extremely slow for large matrices | Low | Low | Acceptable for n ≤ 100; document as limitation; future optimization in PLAN-008 |
| Least-squares residual interpretation unclear to users | Low | Low | Document in header: "For overdetermined systems, returns least-squares solution" |
| Test cases need hand-computed expected values | Low | Low | Use online matrix decomposition tools (e.g., WolframAlpha, numpy) to verify |

---

## 7. Success Criteria

- [ ] **G1 verified:** `Cholesky::decompose(A)` returns L; `A - L*L^T ≈ 0` (within 1e-9)
- [ ] **G2 verified:** `Cholesky::solve(A, b)` produces solution; `A*x - b ≈ 0`
- [ ] **G3 verified:** `QR::decompose(A)` returns Q (orthogonal), R (upper triangular); `A - Q*R ≈ 0`
- [ ] **G4 verified:** `QR::solve()` solves square systems and overdetermined least-squares
- [ ] **G5 verified:** Non-SPD input throws; rank-deficient detected (if implemented)
- [ ] **G6 verified:** ≥ 12 new tests added; all pass
- [ ] **G7 verified:** Total test count ≥ 64; `ctest` passes 100%

---

## 8. References

- `specs.md` — Feature 5 (New Solvers)
- Golub & Van Loan, "Matrix Computations" (4th ed.) — Chapters 2–5 (standard reference)
- Trefethen & Bau, "Numerical Linear Algebra" — Cholesky, QR lectures
- `library/MatrixV1/include/solvers/LUDecomposition.hpp` — existing decomposition pattern to follow
- `library/MatrixV1/tests/test_matrix.cpp` — testing conventions (Catch2, helper functions)