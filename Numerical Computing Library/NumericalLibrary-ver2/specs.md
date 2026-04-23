# Matrix — Project Specification

> **Version:** 1.0
> **Created:** 2026-04-21
> **Last Updated:** 2026-04-23
> **Status:** 🟢 Active

---

## Current Focus

- Spec v1.0 created. Awaiting Plan-001 approval before any feature work begins.

---

## ⚠️ Critical Pre-Work (Do Before Any Feature Work)

- [ ] Add `CMakeLists.txt` — project currently has no build system
- [ ] Fix CRLF line endings across all source files (Windows-style `\r\n` detected)
- [ ] Establish folder layout for `AGENT_MD/` documentation alongside source
- [ ] Confirm target C++ standard (assumed C++17 — confirm before building)

---

## Project Overview

**Matrix** is a C++ numerical computing library and console application for matrix operations, linear equation solving, and numerical methods. It provides a clean, modular architecture where each algorithm lives in its own class, and a menu-driven `main.cpp` acts as the interactive front-end.

The project is a learning and utility platform — designed to be extended with new algorithms, improved with modern C++ idioms, and hardened with a proper build system and automated test suite.

### Problem Statement

Manual matrix computation is tedious and error-prone. Students and engineers working in numerical methods need a reliable, extensible tool that covers the standard algorithms — basic operations, linear solvers, eigenvalue methods, and interpolation — in one place, with verifiable correctness.

### Target Users

- Students studying numerical methods / linear algebra
- Developers learning C++ through mathematical implementations
- Engineers needing a lightweight, dependency-free matrix toolkit

### Current Operational Reality (as of 2026-04-23)

The project is **functional but unpolished**. It compiles and runs as a console menu (`main.cpp`). All 13 menu options work. However:

- There is **no build system** — compilation relies on manual `g++` invocations
- There is **no automated test suite** — only 4 raw `.txt` input files used for manual runs
- The `Matrix` class has **no operator overloading, no bounds checking, no utility methods** (transpose, determinant, inverse)
- `Divide` is **hardcoded to 2×2 matrices** only
- Iterative solvers (Jacobi, Seidel) use a **fixed iteration count** with no convergence check
- `main.cpp` has **inconsistent indentation** throughout
- Source files use **Windows CRLF** line endings

---

## Success Criteria (v2.0 — Full Upgrade)

- [ ] Project builds cleanly via `cmake --build` with zero warnings (`-Wall -Wextra`)
- [ ] All algorithms pass automated unit tests (target: ≥ 90% coverage on core logic)
- [ ] `Matrix` class supports operator overloading (`+`, `-`, `*`, `==`, `<<`)
- [ ] `Matrix` class provides `transpose()`, `determinant()`, `inverse()` for N×N
- [ ] `Divide` generalised beyond 2×2 using the full `inverse()` method
- [ ] Iterative solvers (Jacobi, Seidel) converge by tolerance — not fixed iterations
- [ ] Consistent code style across all files (clang-format compatible)
- [ ] All line endings normalised to LF

---

## Constraints & Non-Negotiables

- **Language:** C++ (standard: C++17 assumed — confirm with owner)
- **Dependencies:** Zero external libraries for core matrix logic (STL only)
- **Test framework:** Catch2 (header-only) — acceptable single-file dependency
- **Build:** CMake ≥ 3.15
- **Platform:** Cross-platform (Linux + Windows; currently only Windows `.exe` present)
- **Non-negotiables:** All algorithms must remain mathematically correct after refactor

---

## Tech Stack

### Current (what exists today)

| Layer | Technology | Notes |
|---|---|---|
| Language | C++ (standard unknown, likely C++14) | CRLF line endings detected |
| Build | None | Manual `g++` only |
| Testing | None | 4 manual `.txt` test input files |
| I/O | `std::cin` / `std::cout` / `std::fstream` | Console menu in `main.cpp` |
| Data structure | `std::vector<std::vector<double>>` | Inside `Matrix` class |

### Target (what we are building toward)

| Layer | Technology | Notes |
|---|---|---|
| Language | C++17 | Enable structured bindings, `if constexpr`, etc. |
| Build | CMake ≥ 3.15 | Separate library target + app target |
| Testing | Catch2 v3 (header-only) | Lives in `tests/` folder |
| Formatting | clang-format | Enforce consistent style |
| I/O | Same console menu, improved UX | Cleaner prompts, convergence feedback |

---

## Architectural Principles

- **One class per algorithm** — no mega-files; each solver/operation has its own `.hpp`/`.cpp` pair
- **Static methods for stateless algorithms** — `Add::compute()`, `GaussElimination::solve()` style preserved
- **Matrix class owns data** — all algorithms receive `const Matrix&` references, never raw arrays
- **No external dependencies in `include/`** — STL only; Catch2 only in `tests/`
- **TDD for all new code** — write tests before implementation; tests live in `tests/` and match source structure
- **Backwards-compatible API** — existing `main.cpp` menu calls must continue to compile after refactors

---

## Codebase Inventory

| File | Role | Status | Last Updated |
|---|---|---|---|
| `Matrix/include/Matrix.hpp` | Core Matrix class interface | ✅ Active | 2026-04-08 |
| `Matrix/src/Matrix.cpp` | Core Matrix class implementation | ✅ Active | 2026-04-08 |
| `Matrix/include/operations/Add.hpp` | Matrix addition interface | ✅ Active | 2026-04-08 |
| `Matrix/src/operations/Add.cpp` | Matrix addition implementation | ✅ Active | 2026-04-08 |
| `Matrix/include/operations/Subtract.hpp` | Matrix subtraction interface | ✅ Active | 2026-04-08 |
| `Matrix/src/operations/Subtract.cpp` | Matrix subtraction implementation | ✅ Active | 2026-04-08 |
| `Matrix/include/operations/Multiply.hpp` | Matrix multiplication interface | ✅ Active | 2026-04-08 |
| `Matrix/src/operations/Multiply.cpp` | Matrix multiplication implementation | ✅ Active | 2026-04-08 |
| `Matrix/include/operations/Divide.hpp` | Matrix division interface (2×2 only) | ⚠️ Limited | 2026-04-08 |
| `Matrix/src/operations/Divide.cpp` | Matrix division — hardcoded 2×2 inverse | ⚠️ Limited | 2026-04-08 |
| `Matrix/include/solvers/SolveLinearEquation.hpp` | Gauss Elimination / Jacobi / Seidel interfaces | ✅ Active | 2026-04-08 |
| `Matrix/src/solvers/GaussElimination.cpp` | Gauss elimination with partial pivoting | ✅ Active | 2026-04-08 |
| `Matrix/src/solvers/GaussJacobi.cpp` | Jacobi iteration (fixed iterations, no convergence check) | ⚠️ Limited | 2026-04-08 |
| `Matrix/src/solvers/GaussSeidel.cpp` | Gauss-Seidel iteration (fixed iterations, no convergence check) | ⚠️ Limited | 2026-04-08 |
| `Matrix/include/solvers/LUDecomposition.hpp` | LU decomposition interface | ✅ Active | 2026-04-08 |
| `Matrix/src/solvers/LUDecomposition.cpp` | Doolittle LU decomposition + solve | ✅ Active | 2026-04-08 |
| `Matrix/include/numerical/Eigen.hpp` | Gershgorin eigenvalue estimator interface | ✅ Active | 2026-04-08 |
| `Matrix/src/numerical/Eigen.cpp` | Gershgorin circle theorem implementation | ✅ Active | 2026-04-08 |
| `Matrix/include/numerical/EigenPower.hpp` | Power method interface | ✅ Active | 2026-04-08 |
| `Matrix/src/numerical/EigenPower.cpp` | Power method for dominant eigenvalue | ✅ Active | 2026-04-08 |
| `Matrix/include/numerical/Interpolation.hpp` | Lagrange interpolation interface | ✅ Active | 2026-04-08 |
| `Matrix/src/numerical/Interpolation.cpp` | Lagrange interpolation implementation | ✅ Active | 2026-04-08 |
| `Matrix/main.cpp` | Console menu driver — 13 options | ✅ Active (messy indent) | 2026-04-09 |
| `Matrix/test_cases/matrix1.txt` | Manual test: 2×3 augmented matrix | 🗂️ Manual only | — |
| `Matrix/test_cases/matrix2.txt` | Manual test: 3×3 linear system | 🗂️ Manual only | — |
| `Matrix/test_cases/matrix3.txt` | Manual test: 3×3 diagonally dominant (Jacobi/Seidel) | 🗂️ Manual only | — |
| `Matrix/test_cases/matrix4.txt` | Manual test: 2×4 (Lagrange points) | 🗂️ Manual only | — |
| `CMakeLists.txt` | Build system | ❌ Missing | — |
| `tests/` | Automated test suite | ❌ Missing | — |
| `AGENT_MD/` | Project documentation framework | ✅ Active | 2026-04-23 |

---

## Feature Index

| # | Feature | Status | Priority | Notes |
|---|---|---|---|---|
| 0 | Pre-Work: Build System + Repo Setup | [x] | P0 | CMakeLists.txt, CRLF fix, folder structure |
| 1 | Matrix Class Modernisation | [x] | P0 | Operator overloading, bounds check, transpose, determinant, inverse |
| 2 | Generalise Divide (N×N) | [x] | P1 | Use `inverse()` from Feature 1; remove 2×2 restriction |
| 3 | Iterative Solver Convergence | [x] | P1 | Add tolerance + max-iter params to Jacobi & Seidel; report iteration count |
| 4 | Automated Test Suite | [x] | P0 | Catch2; cover all existing algorithms + Feature 1 additions |
| 5 | New Solvers | [x] | P2 | Cholesky decomposition, QR decomposition |
| 6 | New Numerical Methods | [x] | P2 | Newton's interpolation, inverse power method (smallest eigenvalue) |
| 7 | CLI / UX Improvements | [x] | P3 | Fix main.cpp indentation, add result saving for solvers, convergence output |
| 8 | Python Mirror Library | [x] | P2 | Full port of MatrixV1 to Python under `library/MatrixV1_Python/`; 104 pytest tests |
| 9 | Least Squares Curve Fitting | [x] | P2 | `LeastSquares::fitLine()` and `fitParabola()` — straight line and parabola via normal equations |

---

## Known Issues & Technical Debt

- `Divide::compute()` — hardcoded `if(B.getRows() != 2 || B.getCols() != 2)` throws for any non-2×2 input (see Feature 2)
- `GaussJacobi::solve()` and `GaussSeidel::solve()` — fixed iteration counts; will produce wrong answers on non-converging systems with no warning (see Feature 3)
- `Matrix::get(i, j)` — no bounds checking; out-of-range access silently corrupts or crashes
- `main.cpp` — indentation is inconsistent throughout; `case 10`–`13` blocks are misaligned
- All source files use Windows CRLF (`\r\n`) line endings
- `LU::solve()` — no check for zero pivot during LU decomposition; can silently produce NaN
- `EigenPower::powerMethod()` — fixed 100 iterations; no convergence check; may return inaccurate result for slowly-converging matrices
- No `Makefile` or `CMakeLists.txt` — building requires manual `g++` command with all files listed

---

## Known Unknowns

- Target C++ standard not confirmed (code appears to be C++11/14; C++17 assumed for target)
- Whether a GUI or web front-end is desired in the long run (console-only for now)
- Performance requirements: are there constraints on matrix size or speed?
- Whether the project will be published / submitted (affects naming, comment style, academic integrity rules)

---

## Decision Log

| Date | Decision | Rationale | Alternatives Considered |
|---|---|---|---|
| 2026-04-23 | Adopted AGENT_MD framework for project documentation | Structured plan/report lifecycle for disciplined, traceable development | Ad-hoc notes |
| 2026-04-23 | Created v1.0 spec.md | Project initialisation — full upgrade in scope | n/a |
| 2026-04-23 | Chose Catch2 v3 as test framework | Header-only, zero external deps, well-known in C++ community | Google Test (heavier setup), manual assert macros |
| 2026-04-23 | Chose CMake ≥ 3.15 as build system | Industry standard; cross-platform; supports separate lib + app targets | Makefile (less portable), Bazel (too heavy) |

---

---

# FEATURE 0 — Pre-Work: Build System & Repo Setup

## Goal

Establish a clean, buildable project baseline before any new features are written. Every subsequent plan depends on having a working CMake build and consistent code formatting.

## Tasks

- [ ] Create `CMakeLists.txt` with separate `matrix_lib` (static library) and `matrix_app` (executable) targets
- [ ] Normalise all line endings to LF (strip `\r`)
- [ ] Set C++ standard to C++17 in CMake
- [ ] Verify clean build with `-Wall -Wextra -Werror`
- [ ] Create `tests/` directory with a minimal Catch2 smoke test

## Verification

`cmake -B build && cmake --build build` exits 0 with zero warnings.

---

# FEATURE 1 — Matrix Class Modernisation

## Goal

Upgrade the `Matrix` class from a bare data container into a first-class numerical object with operator support, safety, and utility methods.

## Additions

- Operator overloading: `+`, `-`, `*`, `==`, `!=`, `<<` (stream output)
- Bounds checking in `get()` and `set()` — throw `std::out_of_range`
- `transpose()` — returns new `Matrix`
- `determinant()` — returns `double` (recursive or LU-based)
- `inverse()` — returns new `Matrix`; throws if singular
- `isSquare()`, `isSymmetric()` utility predicates

## Acceptance Criteria

- [ ] Operator overloads compile and produce correct results
- [ ] `get(-1, 0)` throws `std::out_of_range`
- [ ] `inverse()` of identity returns identity (within 1e-10 tolerance)
- [ ] Existing `main.cpp` still compiles and runs unchanged

---

# FEATURE 2 — Generalise Divide (N×N)

## Goal

Remove the 2×2 restriction from `Divide::compute()` by delegating to `Matrix::inverse()` from Feature 1.

## Acceptance Criteria

- [ ] `Divide::compute(A, B)` works for any N×N invertible `B`
- [ ] Throws meaningfully when `B` is singular or non-square

---

# FEATURE 3 — Iterative Solver Convergence

## Goal

Replace the hardcoded iteration count in Jacobi and Seidel with proper convergence logic.

## Additions

- Add `tolerance` parameter (default `1e-6`) and `maxIterations` (default `1000`)
- Return iteration count alongside solution (or via output param)
- Throw `std::runtime_error` if max iterations reached without convergence

## Acceptance Criteria

- [ ] Solver stops early when residual norm < tolerance
- [ ] Throws when system does not converge within `maxIterations`
- [ ] Existing `main.cpp` calls still work (default params preserve old behaviour)

---

# FEATURE 4 — Automated Test Suite

## Goal

Every algorithm in the library has at least one passing automated test. Tests live in `tests/` and run via `ctest`.

## Coverage targets

| Module | Min tests |
|---|---|
| `Matrix` (core) | 15 |
| Operations (Add/Sub/Mul/Div) | 4 × 3 = 12 |
| Solvers (GaussElim, Jacobi, Seidel, LU) | 4 × 4 = 16 |
| Numerical (Eigen, EigenPower, Interp) | 3 × 3 = 9 |

## Acceptance Criteria

- [ ] `ctest --test-dir build` exits 0 with all tests passing
- [ ] Edge cases covered: singular matrix, zero vector, 1×1 matrix, non-square

---

# FEATURE 5 — New Solvers

## Goal

Add Cholesky decomposition (for symmetric positive-definite systems) and QR decomposition (for least-squares problems).

## Additions

- `Cholesky::decompose(A, L)` and `Cholesky::solve(A, b)`
- `QR::decompose(A, Q, R)` (Gram-Schmidt)

## Acceptance Criteria

- [ ] Cholesky solves a known SPD system correctly
- [ ] QR decomposition satisfies `A == Q * R` within 1e-10 tolerance
- [ ] Throws on non-SPD input for Cholesky

---

# FEATURE 6 — New Numerical Methods

## Goal

Extend the numerical methods module with Newton's divided difference interpolation and the inverse power method for the smallest eigenvalue.

## Additions

- `Interpolation::newton(x, y, value)` — Newton's divided differences
- `EigenPower::inverseMethod(A, eigenvector, iterations)` — smallest eigenvalue via inverse power

## Acceptance Criteria

- [ ] Newton interpolation matches Lagrange result on same data points
- [ ] Inverse power method returns smallest eigenvalue for a known test matrix

---

# FEATURE 7 — CLI / UX Improvements

## Goal

Clean up `main.cpp` indentation, unify the menu style, and add result-saving for solver outputs and convergence feedback for iterative solvers.

## Additions

- Fix all indentation in `main.cpp` (clang-format pass)
- Display iteration count after Jacobi/Seidel solve
- Allow solver solutions to be saved to file (matches matrix output option)
- Add menu separator lines for readability

## Acceptance Criteria

- [ ] `clang-format --dry-run main.cpp` reports no changes
- [ ] Iterative solver output includes: `"Converged in N iterations"`
- [ ] Solver result can be saved to file from menu

---
---

# FEATURE 8 — Python Mirror Library

## Goal

Port the entire MatrixV1 C++ library to Python, preserving the same module structure, algorithm behaviour, and API conventions. Provide an installable package and a `main.py` CLI that mirrors `main.cpp`.

## Additions

- `matrixv1/` Python package under `library/MatrixV1_Python/`
- Modules: `core/matrix.py`, `operations/`, `solvers/`, `numerical/`
- `pyproject.toml` for `pip install -e .`
- 104 pytest tests across 4 test files

## Success Criteria

- [x] `pip install -e .` succeeds in `MatrixV1_Python/`
- [x] `pytest tests/` exits 0 with 104 tests passing
- [x] `python main.py` runs the CLI with all 20 menu options functional

---

# FEATURE 9 — Least Squares Curve Fitting

## Goal

Add least squares curve fitting for a straight line (y = a + bx) and a parabola (y = a + bx + cx²) to both the C++ library and the Python mirror.  Both methods reduce to solving a small system of normal equations using the existing `GaussElimination::solve()` infrastructure.

## Additions

**C++:**
- `include/numerical/LeastSquares.hpp` — `fitLine()` returning `std::pair<double,double>`, `fitParabola()` returning `std::tuple<double,double,double>`
- `src/numerical/LeastSquares.cpp` — single-pass sum accumulation + normal equation solve
- `main.cpp` menu options 21 (line) and 22 (parabola)
- 13 new Catch2 tests

**Python:**
- `matrixv1/numerical/least_squares.py` — `fit_line()`, `fit_parabola()`
- `main.py` menu options 21 and 22
- 16 new pytest tests

## Success Criteria

- [x] `LeastSquares::fitLine({1,2,3,4,5}, {2,4,5,4,5})` returns `{a≈2.2, b≈0.6}` within 1e-6
- [x] `LeastSquares::fitParabola({0,1,2,3}, {1,1.5,3.5,8.0})` returns `{a=1.05, b=-0.7, c=1.0}` within 1e-6
- [x] Both functions throw `std::invalid_argument` on empty, mismatched, or underdetermined input
- [x] `ctest` exits 0 with 97 test cases (81 existing + 13 new in MatrixV1; total was 81+13=94 before smoke tests; with smoke = 97), 0 failures
- [x] `pytest tests/` exits 0 with 120 tests (104 existing + 16 new), 0 failures
- [x] `main.cpp` options 21 and 22 functional; `main.py` options 21 and 22 functional
