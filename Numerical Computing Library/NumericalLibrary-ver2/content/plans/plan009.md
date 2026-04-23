# PLAN-009: Least Squares Fitting — Straight Line & Parabola

**Created:** 2026-04-23
**Status:** Draft
**Addresses:** Feature 9 from specs.md — add least squares curve fitting for a straight line (y = a + bx) and a parabola (y = a + bx + cx²) to both the C++ library and the Python mirror.

---

## 1. Context & Motivation

After PLAN-008 the library has a complete direct-solver stack, three interpolation/eigenvalue methods, and a full Python mirror. However, the **numerical methods module has no curve-fitting capability**. Interpolation (Lagrange, Newton) passes a curve exactly through every data point. Least squares fitting, by contrast, finds the best-fit curve through noisy data by minimising the sum of squared residuals — a fundamental technique in data analysis and engineering.

This plan adds two fitting functions:

- **Straight-line fit:** y = a + bx — 2-parameter model, normal equations solvable as a 2×2 linear system.
- **Parabolic fit:** y = a + bx + cx² — 3-parameter model, normal equations solvable as a 3×3 linear system.

Both methods reduce to solving a small system of **normal equations** derived by differentiating the sum of squared errors with respect to each coefficient and setting to zero. The existing `GaussElimination` solver is used directly, so no new solver infrastructure is needed.

The `current_state_report.md` (§7 Known Issues) and `reconciliation-001.md` (§2.2) both flag this as the next planned feature.

---

## 2. Goals

- **G1:** `LeastSquares::fitLine(x, y)` returns `std::pair<double, double> {a, b}` representing the best-fit line y = a + bx over n data points. Result must satisfy: residual sum of squares is a global minimum (verified by comparing against numpy/manual calculation in tests).
- **G2:** `LeastSquares::fitParabola(x, y)` returns `std::tuple<double, double, double> {a, b, c}` representing the best-fit parabola y = a + bx + cx². Result must agree with known coefficients within 1e-6 on exact-fit test data.
- **G3:** Both functions throw `std::invalid_argument` on:
  - Empty or mismatched x/y vectors.
  - Fewer data points than parameters (n < 2 for line, n < 3 for parabola).
  - Perfectly collinear x values (singular normal-equation matrix).
- **G4:** ≥ 12 new C++ test cases (≥ 5 for fitLine, ≥ 5 for fitParabola, ≥ 2 error handling). All 81 existing C++ tests continue to pass.
- **G5:** Python mirror added: `matrixv1/numerical/least_squares.py` with `fit_line(x, y)` and `fit_parabola(x, y)`. ≥ 12 new pytest cases. All 104 existing Python tests continue to pass.
- **G6:** Both methods integrated into `main.cpp` (options 21 and 22) and `main.py` (equivalent menu entries).
- **G7:** `specs.md` updated with Feature 9. All plan statuses (001–008) corrected to `Completed`.

---

## 3. Non-Goals

- No polynomial fitting beyond degree 2 — generalised polynomial least squares is a separate, larger feature.
- No weighted least squares — all data points have equal weight.
- No R² goodness-of-fit statistic — useful, but out of scope for this plan.
- No matrix-form general linear regression — the normal equations are built manually for the two specific models.
- No changes to existing solver or Matrix class code.

---

## 4. Approach

### 4.1 Mathematical Background

For n data points (xᵢ, yᵢ), least squares minimises:

```
S(a, b) = Σ (yᵢ - a - b·xᵢ)²          [line]
S(a, b, c) = Σ (yᵢ - a - b·xᵢ - c·xᵢ²)²   [parabola]
```

Setting ∂S/∂a = 0, ∂S/∂b = 0 (and ∂S/∂c = 0 for parabola) yields the **normal equations**:

**Straight line (2×2 system):**
```
[ n      Σx  ] [a]   [Σy  ]
[ Σx     Σx² ] [b] = [Σxy ]
```

**Parabola (3×3 system):**
```
[ n      Σx    Σx²  ] [a]   [Σy   ]
[ Σx     Σx²   Σx³  ] [b] = [Σxy  ]
[ Σx²    Σx³   Σx⁴  ] [c]   [Σx²y ]
```

Both systems are solved using the existing `GaussElimination::solve()` function. No new solver logic is required.

### 4.2 C++ Interface Design

New class `LeastSquares` added to the numerical module. Header and source follow the existing pattern (one `.hpp` + one `.cpp`, static methods only, no instantiation needed):

```cpp
// include/numerical/LeastSquares.hpp
#pragma once
#include <vector>
#include <utility>   // std::pair
#include <tuple>     // std::tuple
#include <stdexcept>

class LeastSquares {
public:
    // Fit y = a + bx to data points (x, y).
    // Returns {a, b} — intercept and slope.
    // Throws std::invalid_argument if data is invalid or underdetermined.
    static std::pair<double, double>
    fitLine(const std::vector<double>& x,
            const std::vector<double>& y);

    // Fit y = a + bx + cx² to data points (x, y).
    // Returns {a, b, c} — intercept, linear, quadratic coefficients.
    // Throws std::invalid_argument if data is invalid or underdetermined.
    static std::tuple<double, double, double>
    fitParabola(const std::vector<double>& x,
                const std::vector<double>& y);
};
```

### 4.3 Implementation Details

**Sums computed:** n, Σx, Σx², Σx³, Σx⁴, Σy, Σxy, Σx²y — all computed in a single pass through the data.

**Normal equation matrix built as `Matrix` object**, then solved using the existing `GaussElimination::solve()` static function. This reuses tested infrastructure and avoids duplicating solver code.

```cpp
// fitLine — sketch
std::pair<double, double> LeastSquares::fitLine(
        const std::vector<double>& x,
        const std::vector<double>& y) {
    int n = x.size();
    // Validation ...
    double sx = 0, sx2 = 0, sy = 0, sxy = 0;
    for (int i = 0; i < n; i++) {
        sx  += x[i];
        sx2 += x[i] * x[i];
        sy  += y[i];
        sxy += x[i] * y[i];
    }
    // Build 2x2 normal equation matrix + RHS
    Matrix A(2, 2);
    A.set(0, 0, n);   A.set(0, 1, sx);
    A.set(1, 0, sx);  A.set(1, 1, sx2);
    std::vector<double> b = {sy, sxy};
    // Solve
    std::vector<double> coeffs = GaussElimination::solve(A, b);
    return {coeffs[0], coeffs[1]};
}
```

**Singularity handling:** `GaussElimination::solve()` throws if the matrix is singular (all x values equal → zero-determinant normal matrix). This exception propagates naturally; no extra handling is needed.

### 4.4 Python Interface Design

```python
# matrixv1/numerical/least_squares.py

def fit_line(x: list[float], y: list[float]) -> tuple[float, float]:
    """
    Fit y = a + b*x by least squares.
    Returns (a, b) — intercept and slope.
    Raises ValueError if data is invalid or underdetermined.
    """

def fit_parabola(x: list[float], y: list[float]) -> tuple[float, float, float]:
    """
    Fit y = a + b*x + c*x^2 by least squares.
    Returns (a, b, c).
    Raises ValueError if data is invalid or underdetermined.
    """
```

The Python implementation builds the same normal-equation sums, constructs a Python `Matrix` object, and calls `GaussElimination.solve()` from `matrixv1/solvers/gauss_elimination.py`. Same algorithm, same structure as C++.

### 4.5 Menu Integration

**C++ `main.cpp`** — add a new "Curve Fitting" section after the existing Numerical Methods section:

```
══ CURVE FITTING ══════════════════════
  [21] Least Squares — Straight Line
  [22] Least Squares — Parabola
```

Each option prompts for the number of data points, then reads (x, y) pairs, calls the corresponding `LeastSquares` function, and prints the result in the form:

```
Fitted line:  y = 2.1000 + 3.5000x
Fitted curve: y = 1.2000 + 0.5000x + 2.3000x²
```

**Python `main.py`** — equivalent menu additions at options 21 and 22, same prompts and output format.

### 4.6 Files Changed

| File | Change type | Description |
|------|-------------|-------------|
| `library/MatrixV1/include/numerical/LeastSquares.hpp` | ➕ New | Class declaration |
| `library/MatrixV1/src/numerical/LeastSquares.cpp` | ➕ New | `fitLine()` and `fitParabola()` implementations |
| `library/MatrixV1/CMakeLists.txt` | ✏️ Modified | Add `src/numerical/LeastSquares.cpp` to `matrix_lib` sources |
| `library/MatrixV1/tests/test_matrix.cpp` | ✏️ Modified | Add ≥ 12 new `TEST_CASE` blocks |
| `library/MatrixV1/main.cpp` | ✏️ Modified | Add options 21 & 22 to menu and dispatch |
| `library/MatrixV1_Python/matrixv1/numerical/least_squares.py` | ➕ New | Python mirror |
| `library/MatrixV1_Python/matrixv1/numerical/__init__.py` | ✏️ Modified | Export `fit_line`, `fit_parabola` |
| `library/MatrixV1_Python/tests/test_numerical.py` | ✏️ Modified | Add ≥ 12 pytest cases |
| `library/MatrixV1_Python/main.py` | ✏️ Modified | Add options 21 & 22 to Python CLI menu |
| `specs.md` | ✏️ Modified | Add Feature 9, fix stray `]`, update Last Updated |
| `content/plans/plan001.md` – `plan008.md` | ✏️ Modified | Change status to `Completed` (8 × 1-line change) |

---

## 5. Task Breakdown

| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | Correct plan001–plan008 status fields to `Completed`; fix stray `]` in specs.md | 10 min | — |
| 2 | Add Feature 9 to `specs.md` (Feature Index + Success Criteria) | 10 min | 1 |
| 3 | Write C++ tests in `test_matrix.cpp`: ≥ 12 TEST_CASE blocks for `fitLine` and `fitParabola` (write first — TDD) | 40 min | — |
| 4 | Create `include/numerical/LeastSquares.hpp` — class declaration and doc comments | 15 min | — |
| 5 | Create `src/numerical/LeastSquares.cpp` — implement `fitLine()` | 30 min | 4 |
| 6 | Create `src/numerical/LeastSquares.cpp` — implement `fitParabola()` | 25 min | 5 |
| 7 | Add `src/numerical/LeastSquares.cpp` to `CMakeLists.txt` `matrix_lib` sources | 5 min | 6 |
| 8 | Build and run `ctest` — all tests (old 81 + new ≥12) must pass | 10 min | 3, 7 |
| 9 | Add options 21 & 22 to `main.cpp` (input prompt, call, formatted output) | 20 min | 8 |
| 10 | Create `matrixv1/numerical/least_squares.py` — Python mirror of `fitLine` and `fitParabola` | 30 min | 5, 6 |
| 11 | Update `matrixv1/numerical/__init__.py` to export new functions | 5 min | 10 |
| 12 | Add ≥ 12 pytest cases to `tests/test_numerical.py` for Python functions | 30 min | 10 |
| 13 | Run `pytest tests/` — all tests (old 104 + new ≥12) must pass | 5 min | 12 |
| 14 | Add options 21 & 22 to `main.py` | 20 min | 13 |
| 15 | Update `content/current_state_report.md` with PLAN-009 update block | 10 min | 8, 13 |

**Total estimated time: ~4 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| `GaussElimination::solve()` throws on singular normal matrix without a clear message | Medium | Low | Catch the exception in `fitLine`/`fitParabola` and rethrow as `std::invalid_argument` with a descriptive message ("all x values are identical — normal equation matrix is singular") |
| Floating-point accumulation error in sums for large datasets | Low | Low | Test with moderate n (≤ 100 points); for larger datasets Kahan summation can be added in a future plan |
| Python `Matrix` constructor API differs subtly from C++ for small matrices | Low | Low | Check `matrix.py` `__init__` signature before writing Python implementation; the Python `Matrix` is fully tested |
| Normal equations for parabola are ill-conditioned when x data spans a very wide range | Low | Medium | Not in scope — document as a known limitation; use column normalisation in a future plan if needed |
| Menu option numbering collides if `main.cpp` already uses 21/22 | Low | Low | Verify the current highest option number before adding (PLAN-007 added up to option 20) |

---

## 7. Success Criteria

- [ ] **G1 verified:** `LeastSquares::fitLine({1,2,3,4,5}, {2,4,5,4,5})` returns `{a, b}` such that `a ≈ 2.2` and `b ≈ 0.6` (matches standard OLS result within 1e-6)
- [ ] **G2 verified:** `LeastSquares::fitParabola({0,1,2,3}, {1,1.5,3.5,8.0})` returns `{a,b,c}` such that `a ≈ 1.0`, `b ≈ -0.35`, `c ≈ 0.85` (or exact for exact-fit data) within 1e-6
- [ ] **G3 verified:** `fitLine({}, {})` throws `std::invalid_argument`; `fitLine({1,2}, {3})` throws; `fitLine({2,2,2}, {1,2,3})` throws (singular)
- [ ] **G4 verified:** `ctest --test-dir build --output-on-failure` exits 0 with ≥ 93 test cases (81 existing + ≥ 12 new), 0 failures
- [ ] **G5 verified:** `pytest library/MatrixV1_Python/tests/ -v` exits 0 with ≥ 116 tests (104 existing + ≥ 12 new), 0 failures
- [ ] **G6 verified:** `main.cpp` compiled binary responds to menu option 21 with line fit output; option 22 with parabola fit output
- [ ] **G7 verified:** `grep "Status:" content/plans/plan00*.md` returns `Completed` for all 8 lines; `specs.md` contains Feature 9 entry; no stray `]` on line 12 of `specs.md`

---

## 8. References

- `specs.md` — Feature 9 (to be added); Features 5 & 6 (existing numerical methods for structural reference)
- `content/current_state_report.md` — §7 Known Issues (Least Squares missing)
- `reconciliation-001.md` — §2.2 (Least Squares not in original spec; action plan to add Feature 9)
- `library/MatrixV1/src/numerical/Interpolation.cpp` — structural pattern for new numerical method
- `library/MatrixV1/src/solvers/GaussElimination.cpp` — solver used by normal equations
- `library/MatrixV1_Python/matrixv1/numerical/interpolation.py` — Python mirror pattern to follow
- Burden, R.L. & Faires, J.D. — *Numerical Analysis*, §8.1 (Discrete Least Squares Approximation)