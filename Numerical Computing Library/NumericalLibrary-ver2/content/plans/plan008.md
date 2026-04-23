# PLAN-008: Port Library to Python

**Created:** 2026-04-22
**Status:** Draft
**Addresses:** Translate the entire MatrixV1 C++ library into an idiomatic Python package — same algorithms, same module structure, same test coverage — with a Python-native interface.

---

## 1. Context & Motivation

MatrixV1 is feature-complete in C++. PLAN-008 creates a parallel Python implementation. This is motivated by:

- **Accessibility** — Python lowers the barrier for students and researchers who need the algorithms without a C++ toolchain.
- **Interoperability** — a Python package integrates naturally with NumPy, SciPy, and Jupyter notebooks.
- **Maintainability** — Python's dynamic typing and built-in `list`/`numpy` support simplify matrix I/O and testing.
- **Scope** — the C++ implementation is the reference. The Python port must produce identical numerical results on all shared test cases (within floating-point tolerance).

---

## 2. Goals

- **G1:** Produce a Python package `matrixv1/` that mirrors the C++ module structure: `core`, `operations`, `solvers`, `numerical`.
- **G2:** `Matrix` class in Python wraps a 2D `list[list[float]]` (or optionally `numpy.ndarray`) with the same public interface as the C++ class: `get`, `set`, `transpose`, `determinant`, `inverse`, `is_square`, `is_symmetric`, operator overloads (`+`, `-`, `*`, `==`).
- **G3:** All C++ operation classes ported: `Add`, `Subtract`, `Multiply`, `Divide`.
- **G4:** All C++ solver classes ported: `GaussElimination`, `LU`, `Cholesky`, `QR`, `GaussJacobi`, `GaussSeidel`.
- **G5:** All C++ numerical classes ported: `EigenPower` (power method + inverse method), `Interpolation` (Lagrange + Newton).
- **G6:** A pytest test suite mirrors `test_matrix.cpp` — same scenarios, ≥ 81 tests, all passing.
- **G7:** A `main.py` CLI script mirrors the C++ `main.cpp` menu — all 20 options functional.
- **G8:** Package is installable via `pip install -e .` with a `pyproject.toml`.
- **G9:** Zero external dependencies for the core library (stdlib only). NumPy optional for I/O convenience.
- **G10:** All algorithms produce results matching the C++ reference within `1e-9` tolerance on shared test inputs.

---

## 3. Non-Goals

- No Python bindings wrapping the C++ code (pybind11, ctypes, Cython) — this is a clean Python reimplementation.
- No GPU acceleration or sparse matrix support.
- No async/parallel execution.
- No web API or notebook integration — just a local package.
- No performance parity with C++ — correctness and clarity over speed.

---

## 4. Package Structure

```
matrixv1/
├── pyproject.toml                  ← package metadata, build config
├── README.md                       ← usage examples
├── main.py                         ← CLI menu (mirrors main.cpp)
├── matrixv1/
│   ├── __init__.py                 ← re-exports Matrix + all public classes
│   ├── core/
│   │   ├── __init__.py
│   │   └── matrix.py               ← Matrix class
│   ├── operations/
│   │   ├── __init__.py
│   │   ├── add.py
│   │   ├── subtract.py
│   │   ├── multiply.py
│   │   └── divide.py
│   ├── solvers/
│   │   ├── __init__.py
│   │   ├── gauss_elimination.py
│   │   ├── lu_decomposition.py
│   │   ├── cholesky.py
│   │   ├── qr.py
│   │   ├── gauss_jacobi.py
│   │   └── gauss_seidel.py
│   └── numerical/
│       ├── __init__.py
│       ├── eigen.py
│       ├── eigen_power.py
│       └── interpolation.py
└── tests/
    ├── test_core.py
    ├── test_operations.py
    ├── test_solvers.py
    └── test_numerical.py
```

---

## 5. Module-by-Module Mapping

### 5.1 `Matrix` class (`matrixv1/core/matrix.py`)

| C++ | Python |
|-----|--------|
| `Matrix(int r, int c)` | `Matrix(r: int, c: int)` |
| `get(i, j)` | `get(i, j)` — raises `IndexError` |
| `set(i, j, val)` | `set(i, j, val)` — raises `IndexError` |
| `getRows()` / `getCols()` | `.rows` / `.cols` properties |
| `isSquare()` | `is_square()` |
| `isSymmetric()` | `is_symmetric(tol=1e-9)` |
| `transpose()` | `transpose()` |
| `determinant(DetMethod)` | `determinant(method='lu')` — `'lu'` or `'cofactor'` |
| `inverse()` | `inverse()` — raises `ValueError` if singular |
| `input()` | `input_from_console()` |
| `inputFromFile(fn)` | `from_file(filename)` classmethod |
| `display()` | `__str__` / `__repr__` |
| `saveToFile(fn)` | `to_file(filename)` |
| `operator+` | `__add__` |
| `operator-` | `__sub__` |
| `operator*` | `__mul__` |
| `operator==` | `__eq__` with `tol=1e-9` |
| `operator<<` | `__str__` |

**Internal representation:** `self._data: list[list[float]]` — no NumPy dependency in core, but a `Matrix.from_numpy(arr)` classmethod is provided for convenience.

### 5.2 Operations (`matrixv1/operations/`)

Each is a module-level function (not a class with a static method, which is un-Pythonic):

```python
# add.py
def add(A: Matrix, B: Matrix) -> Matrix: ...

# divide.py
def divide(A: Matrix, B: Matrix) -> Matrix:
    return multiply(A, B.inverse())
```

The `Matrix` class `__add__` etc. delegate to these functions, mirroring the C++ design.

### 5.3 Solvers (`matrixv1/solvers/`)

All solver functions return `list[float]` (solution vector x), matching the C++ `std::vector<double>` return type.

| C++ class | Python module | Function signature |
|---|---|---|
| `GaussElimination::solve(A, b)` | `gauss_elimination.py` | `solve(A, b)` |
| `LU::decompose(A, L, U)` | `lu_decomposition.py` | `decompose(A) -> (L, U)` |
| `LU::solve(A, b)` | `lu_decomposition.py` | `solve(A, b)` |
| `Cholesky::decompose(A)` | `cholesky.py` | `decompose(A) -> L` |
| `Cholesky::solve(A, b)` | `cholesky.py` | `solve(A, b)` |
| `QR::decompose(A)` | `qr.py` | `decompose(A) -> (Q, R)` |
| `QR::solve(A, b)` | `qr.py` | `solve(A, b)` |
| `GaussJacobi::solve(A, b, tol, maxIter, iters)` | `gauss_jacobi.py` | `solve(A, b, tol=1e-6, max_iter=1000) -> (x, iters)` |
| `GaussSeidel::solve(A, b, tol, maxIter, iters)` | `gauss_seidel.py` | `solve(A, b, tol=1e-6, max_iter=1000) -> (x, iters)` |

Note: Python iterative solvers return `(solution, iters)` as a tuple instead of an output-parameter — more idiomatic than a reference parameter.

### 5.4 Numerical (`matrixv1/numerical/`)

| C++ | Python |
|---|---|
| `Interpolation::lagrange(x, y, value)` | `interpolation.lagrange(x, y, value)` |
| `Interpolation::newton(x, y, value)` | `interpolation.newton(x, y, value)` |
| `EigenPower::powerMethod(A, vec, iters)` | `eigen_power.power_method(A, iterations=100) -> (eigenvalue, eigenvector)` |
| `EigenPower::inverseMethod(A, vec, tol, maxIter, iters)` | `eigen_power.inverse_method(A, tol=1e-9, max_iter=1000) -> (eigenvalue, eigenvector, iters)` |
| `Eigen::gershgorin(A)` | `eigen.gershgorin(A)` — prints estimates |

---

## 6. Testing Strategy

The pytest suite in `tests/` mirrors `test_matrix.cpp` test-for-test. Each C++ `TEST_CASE` maps to one Python `def test_...()` function.

**Test files:**

| File | Coverage | Min tests |
|---|---|---|
| `test_core.py` | `Matrix` class — construction, get/set, bounds, predicates, transpose, determinant, inverse, operators | 38 |
| `test_operations.py` | Add, Subtract, Multiply, Divide | 12 |
| `test_solvers.py` | All 6 solvers; convergence; edge cases | 25 |
| `test_numerical.py` | Lagrange, Newton, EigenPower × 2 | 11 |

**Cross-validation approach:** For every solver and method, the Python test uses the same numerical inputs as the C++ `test_matrix.cpp` test cases and asserts the result within `1e-9` tolerance. This guarantees the Python port is numerically consistent with the C++ reference.

---

## 7. Task Breakdown

| # | Task | Est. |
|---|------|------|
| 1 | Set up package scaffold: `pyproject.toml`, folder structure, `__init__.py` files | 20 min |
| 2 | Implement `Matrix` class with full interface | 60 min |
| 3 | Write `test_core.py` — 38 tests | 40 min |
| 4 | Implement `operations/` (Add, Subtract, Multiply, Divide) | 20 min |
| 5 | Write `test_operations.py` — 12 tests | 20 min |
| 6 | Implement `solvers/gauss_elimination.py` | 20 min |
| 7 | Implement `solvers/lu_decomposition.py` | 20 min |
| 8 | Implement `solvers/cholesky.py` | 25 min |
| 9 | Implement `solvers/qr.py` (Modified Gram-Schmidt) | 30 min |
| 10 | Implement `solvers/gauss_jacobi.py` + `gauss_seidel.py` | 30 min |
| 11 | Write `test_solvers.py` — 25 tests | 40 min |
| 12 | Implement `numerical/interpolation.py` | 20 min |
| 13 | Implement `numerical/eigen_power.py` + `eigen.py` | 30 min |
| 14 | Write `test_numerical.py` — 11 tests | 25 min |
| 15 | Implement `main.py` CLI (20 menu options) | 45 min |
| 16 | `pip install -e .` verification; run full pytest suite | 20 min |

**Total estimated time: ~7 hours**

---

## 8. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Python list-based Matrix slow for large N | Low | Low | Acceptable — correctness over performance; document if needed |
| Floating-point differences between C++ `double` and Python `float` | Low | Low | Both are IEEE 754 64-bit; same algorithms should agree within 1e-12 |
| LU decomposition without pivoting diverges on some inputs | Low | Medium | Mirror the C++ implementation exactly — no pivoting in LU (consistent with reference) |
| QR Modified Gram-Schmidt loses orthogonality on ill-conditioned matrices | Low | Low | Same limitation as C++ reference; document |
| `pyproject.toml` build backend differences | Low | Low | Use `setuptools` with `find_packages`; well-supported |
| Python does not have `std::vector<double>` output params | Low | Low | Return tuples instead — e.g. `solve(A, b) -> (x, iters)` |

---

## 9. Success Criteria

- [ ] **G1:** `import matrixv1` works after `pip install -e .`
- [ ] **G2:** `Matrix` class passes all 38 core tests
- [ ] **G3–G5:** All operation, solver, and numerical modules pass their tests
- [ ] **G6:** `pytest tests/` exits 0 with ≥ 81 tests passing
- [ ] **G7:** `python main.py` launches the menu; all 20 options functional
- [ ] **G8:** `pip install -e .` completes without error
- [ ] **G9:** `import matrixv1` with no NumPy installed works (core only)
- [ ] **G10:** `python -c "from matrixv1 import Matrix; ..."` produces same result as C++ for all shared test vectors

---

## 10. References

- `specs.md` — project background and architectural principles (adapted for Python)
- `report007.md` — MatrixV1 C++ feature-complete baseline
- `library/MatrixV1/src/` — C++ reference implementation (all algorithms)
- `library/MatrixV1/tests/test_matrix.cpp` — C++ test cases to mirror in pytest
- Python `pyproject.toml` docs: https://packaging.python.org/en/latest/guides/writing-pyproject-toml/
- pytest docs: https://docs.pytest.org