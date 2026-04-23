# MatrixV1 Numerical Library — Current State Report

**Date:** 2026-04-23
**Prepared for:** Project re-onboarding, next-plan planning, AI agent context
**Scope:** Full audit of `library/MatrixV1/` (C++), `library/MatrixV1_Python/` (Python), and `content/` documentation after completion of PLAN-001 through PLAN-008.

---

<!--
  HOW TO USE THIS FILE:

  This is a LIVING DOCUMENT. Update it after every completed plan.
  New updates go at the TOP (reverse chronological), below this comment block.

  Format each update as:

  ## Update — YYYY-MM-DD [PLAN-NNN Title]
  - Bullet summary of what changed
  - C++ test results: X passed, Y failed
  - Python test results: X passed, Y failed
  - Key files created/modified

  The numbered sections below (1–7) represent the baseline after PLAN-008.
  Update them as plans are completed.
-->

---

## Update — 2026-04-23 [PLAN-008: Port Library to Python]

- Full Python package `matrixv1/` created under `library/MatrixV1_Python/`
- Mirrors all C++ modules: `core/`, `operations/`, `solvers/`, `numerical/`
- `pip install -e .` installs the package; `python main.py` runs the CLI
- C++ tests: 81 passing, 0 failed
- Python tests: 104 passing, 0 failed
- Key files: `matrixv1/core/matrix.py`, all solver/numerical/operation modules, `pyproject.toml`

## Update — 2026-04-23 [PLAN-007: CLI / UX Improvements]

- `main.cpp` fully rewritten — uniform indentation, boxed 4-section menu, 20 options
- Convergence iteration count printed for Jacobi and Seidel solvers
- File-save option added for vector results
- C++ tests: 81 passing, 0 failed (no library code modified)

## Update — 2026-04-23 [PLAN-006: Newton Interpolation & Inverse Power Method]

- `Interpolation::newton()` added (in-place divided difference table + Horner evaluation)
- `EigenPower::inverseMethod()` added (LU-solve-based; tolerance + max-iter convergence)
- C++ tests: 81 passing, 0 failed (+11 new: 6 Newton, 5 inverse power)

## Update — 2026-04-23 [PLAN-005: New Solvers — Cholesky & QR]

- `Cholesky::decompose()` and `Cholesky::solve()` added for symmetric positive-definite matrices
- `QR::decompose()` added using Modified Gram-Schmidt; `QR::solve()` added for least-norm problems
- C++ tests: 70 passing, 0 failed (+14 new: 7 Cholesky, 7 QR)

## Update — 2026-04-23 [PLAN-004: Iterative Solver Convergence]

- `GaussJacobi` and `GaussSeidel` updated to tolerance-based convergence (old fixed-iteration API preserved for backward compatibility)
- Iteration count available via reference parameter overload
- C++ tests: 56 passing, 0 failed (+7 new)

## Update — 2026-04-23 [PLAN-003: Generalise Divide]

- `Divide::compute()` now handles N×N matrices via `B.inverse()` (was hardcoded 2×2)
- C++ tests: 49 passing, 0 failed (+5 new)

## Update — 2026-04-23 [PLAN-002: Matrix Class Modernisation]

- Operators `+` `-` `*` `==` `!=` `<<` added (delegate to static operation classes)
- Bounds-checked `get()` / `set()` (throw `std::out_of_range`)
- `transpose()`, `determinant()` (LU + Cofactor), `inverse()` (Gauss-Jordan), `isSquare()`, `isSymmetric()` added
- C++ tests: 44 passing, 0 failed (+38 new)

## Update — 2026-04-23 [PLAN-001: Build System & Repo Setup]

- CMake project created: `matrix_lib` (static library), `matrix_app` (executable), `matrix_tests` (Catch2)
- Catch2 v3-compatible placeholder integrated; `ctest` discovers and runs tests
- CRLF → LF on all 23 source files; `.gitattributes` added
- C++ tests: 6 passing (smoke tests), 0 failed

---

## 1. Executive Summary

The MatrixV1 project is in excellent shape as of 2026-04-23. **Both the C++ library and its Python mirror are fully functional, well-tested, and buildable from scratch.**

The C++ library (`library/MatrixV1/`) builds cleanly via CMake with zero compiler warnings under `-Wall -Wextra -Wpedantic`. It provides a complete matrix operations stack — basic arithmetic, six linear solvers, three numerical methods, and a modernised `Matrix` class — backed by **81 Catch2 tests, all passing**.

The Python library (`library/MatrixV1_Python/`) is a direct port, installable via `pip install -e .`, with **104 pytest tests, all passing** and a `main.py` CLI matching the C++ menu.

The next natural extension is **Least Squares fitting** (PLAN-009) — currently absent from both libraries.

Outstanding housekeeping: plan status fields in `content/plans/` were not updated as plans progressed; all eight plans still show `Draft` or `In Progress` rather than `Completed`. This should be corrected before PLAN-009 begins.

---

## 2. C++ Source Inventory (`library/MatrixV1/`)

### Core class

| File | Role |
|------|------|
| `include/Matrix.hpp` | `Matrix` class declaration — constructors, operators, utility methods |
| `src/Matrix.cpp` | `Matrix` implementation — `get`, `set`, bounds checking, determinant (LU + cofactor), inverse (Gauss-Jordan), transpose, `isSquare`, `isSymmetric` |

### Operations (`include/operations/` + `src/operations/`)

| Module | Function |
|--------|----------|
| `Add` | Element-wise matrix addition |
| `Subtract` | Element-wise matrix subtraction |
| `Multiply` | Matrix multiplication (standard O(n³)) |
| `Divide` | A × B⁻¹ (N×N generalised via `B.inverse()`) |

### Solvers (`include/solvers/` + `src/solvers/`)

| Module | Algorithm |
|--------|-----------|
| `GaussElimination` | Forward elimination + back substitution |
| `GaussJacobi` | Jacobi iterative method (tolerance + max-iter convergence) |
| `GaussSeidel` | Gauss-Seidel iterative method (same interface as Jacobi) |
| `LUDecomposition` | Doolittle LU factorisation + forward/back substitution |
| `Cholesky` | Cholesky decomposition for SPD matrices; `decompose()` + `solve()` |
| `QR` | QR decomposition via Modified Gram-Schmidt; `decompose()` + `solve()` |

### Numerical methods (`include/numerical/` + `src/numerical/`)

| Module | Methods |
|--------|---------|
| `Eigen` | `jacobi()` — symmetric eigenvalue solver |
| `EigenPower` | `powerMethod()` (dominant eigenvalue), `inverseMethod()` (smallest eigenvalue) |
| `Interpolation` | `lagrange()` (Lagrange interpolation), `newton()` (Newton divided differences) |

### Build & tests

| Component | Details |
|-----------|---------|
| Build system | CMake 3.28, C++17, 3 targets (`matrix_lib`, `matrix_app`, `matrix_tests`) |
| Test framework | Catch2 v3 (placeholder amalgamated; replace with real Catch2 when network available) |
| Test count | **81 test cases** across `tests/test_matrix.cpp` + `tests/test_smoke.cpp` |
| Test result | ✅ All passing, 0 failures |
| Compiler warnings | 0 under `-Wall -Wextra -Wpedantic` |

---

## 3. Python Source Inventory (`library/MatrixV1_Python/`)

### Package structure (`matrixv1/`)

| Module path | C++ equivalent | Tests |
|-------------|----------------|-------|
| `core/matrix.py` | `Matrix.hpp/.cpp` | `tests/test_core.py` (44 tests) |
| `operations/add.py` | `Add.cpp` | `tests/test_operations.py` (14 tests) |
| `operations/subtract.py` | `Subtract.cpp` | |
| `operations/multiply.py` | `Multiply.cpp` | |
| `operations/divide.py` | `Divide.cpp` | |
| `solvers/gauss_elimination.py` | `GaussElimination.cpp` | `tests/test_solvers.py` (30 tests) |
| `solvers/gauss_jacobi.py` | `GaussJacobi.cpp` | |
| `solvers/gauss_seidel.py` | `GaussSeidel.cpp` | |
| `solvers/lu_decomposition.py` | `LUDecomposition.cpp` | |
| `solvers/cholesky.py` | `Cholesky.cpp` | |
| `solvers/qr.py` | `QR.cpp` | |
| `numerical/eigen.py` | `Eigen.cpp` | `tests/test_numerical.py` (16 tests) |
| `numerical/eigen_power.py` | `EigenPower.cpp` | |
| `numerical/interpolation.py` | `Interpolation.cpp` | |

### Build & tests

| Component | Details |
|-----------|---------|
| Install | `pip install -e .` |
| Test runner | pytest |
| Test count | **104 tests** across 4 test files |
| Test result | ✅ All passing, 0 failures (`104 passed in 0.38s`) |
| Python version | 3.13 (CPython) |

---

## 4. Test Suite Status

| Suite | Passed | Failed | Skipped | Framework |
|-------|-------:|-------:|--------:|-----------|
| C++ — `matrix_tests` (Catch2) | 81 | 0 | 0 | ctest |
| Python — pytest | 104 | 0 | 0 | pytest |
| **Total** | **185** | **0** | **0** | |

---

## 5. Infrastructure & Build

| Component | Status | Notes |
|-----------|--------|-------|
| CMake build (C++) | ✅ Working | `cmake -B build && cmake --build build` exits 0 |
| ctest (C++) | ✅ Working | `ctest --test-dir build` exits 0, 81/81 pass |
| pip install (Python) | ✅ Working | `pip install -e .` in `MatrixV1_Python/` |
| pytest (Python) | ✅ Working | 104/104 pass |
| CI pipeline | ❌ None | No GitHub Actions or equivalent. Manual build only. |
| Real Catch2 v3 | ⚠️ Placeholder | Network allowlist blocked download. Replace when available. |
| `.gitattributes` | ✅ Present | LF enforced for all `.cpp`, `.hpp`, `.md`, `.txt` |
| Line endings | ✅ LF | All 23 C++ source files confirmed LF |

---

## 6. Documentation Status

| Document | Status | Notes |
|----------|--------|-------|
| `specs.md` | ✅ Present | Covers features 0–7 (+ Python port). Minor stray `]` at line 12. |
| `content/rules.md` | ✅ Added (this session) | Authoring conventions for plans/reports |
| `content/current_state_report.md` | ✅ This file | |
| `content/plans/plan001–008.md` | ✅ Present | All 8 plans written |
| `content/report/report001–008.md` | ✅ Present | All 8 reports written |
| Plan status fields | ⚠️ Stale | All 8 plans still show `Draft` or `In Progress`; should be `Completed` |
| `reconciliation-001.md` | ✅ Added (this session) | First cross-audit |

---

## 7. Known Issues & Technical Debt

- **Plan statuses not updated** — All 8 plan files still have `**Status:** Draft` (except plan006 which has `In Progress`). Per `rules.md` §7, all should read `Completed`. Fix before PLAN-009 begins.
- **Catch2 placeholder** — `tests/catch2/` contains a hand-written Catch2-compatible shim, not the real Catch2 v3 amalgamated files. All 81 tests pass with it. Swap in real Catch2 (network needed) when possible.
- **No CI pipeline** — Build and test verification is entirely manual. Adding GitHub Actions is a low-effort improvement.
- **`specs.md` stray character** — A stray `]` appears at line 12 (`]\n\n## Current Focus`). Harmless but should be removed.
- **Least Squares missing** — Neither library implements least squares curve fitting (straight line or parabola). This is the target of PLAN-009.
- **`main.py` menu (Python) hardcoded strings** — Python CLI duplicates several prompt strings from `main.cpp`. A shared constants module was not created. Low priority.
- **No `requirements.txt` lock** — `requirements.txt` in `MatrixV1_Python/` contains pinned versions from `pip freeze` but was generated on a specific Python 3.13 environment. May differ on other setups.