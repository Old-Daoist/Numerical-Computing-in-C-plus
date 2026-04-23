# PLAN-001: Build System & Repo Setup

**Created:** 2026-04-20
**Status:** Draft
**Addresses:** The project has no build system, no test scaffolding, and Windows-style CRLF line endings throughout. Nothing in Feature 1–7 can be built, tested, or CI-verified until this baseline exists.

---

## 1. Context & Motivation

The Current State Report (`AGENT_MD/spec.md`, Feature 0 and Known Issues) identifies three blocking problems that must be resolved before any feature work begins:

1. **No build system** — there is no `CMakeLists.txt`. Building requires manually listing every `.cpp` file in a `g++` command. This is fragile, non-portable, and incompatible with automated testing.
2. **No test scaffolding** — there is no `tests/` directory, no test runner, and no way to verify algorithm correctness automatically. All subsequent plans depend on being able to write and run tests.
3. **CRLF line endings** — all source files use Windows-style `\r\n`. This causes `git diff` noise, clang-format warnings, and subtle bugs in file-reading code on Linux.

This plan establishes a clean, reproducible build baseline. It does not change any algorithm logic — only the project structure and tooling.

---

## 2. Goals

- **G1:** `cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build` exits 0 with zero warnings under `-Wall -Wextra`.
- **G2:** The project builds as two separate CMake targets: `matrix_lib` (static library containing all algorithm `.cpp` files) and `matrix_app` (executable linking `matrix_lib` and building `main.cpp`).
- **G3:** A `tests/` directory exists with Catch2 v3 (single-header) included and one passing smoke test that verifies the `Matrix` constructor and `getRows()`/`getCols()`.
- **G4:** `ctest --test-dir build` exits 0.
- **G5:** All source files use LF line endings — zero `\r` bytes remain.

---

## 3. Non-Goals

- No algorithm changes — `Matrix`, solvers, operations, and numerical methods are untouched logically.
- No new algorithms — Cholesky, QR, Newton interpolation, etc. are deferred to later plans.
- No operator overloading or Matrix class modernisation — that is PLAN-002 (Feature 1).
- No CI pipeline (GitHub Actions, etc.) — can be added later as a separate plan.
- No clang-format enforcement — style cleanup is PLAN-007 (Feature 7).

---

## 4. Approach

### 4.1 CMake project structure

We introduce a top-level `CMakeLists.txt` that defines two targets:

```
matrix_lib   — static library
  Sources: all .cpp files under Matrix/src/ (recursively)
  Include dirs: Matrix/include/

matrix_app   — executable
  Sources: Matrix/main.cpp
  Links: matrix_lib

matrix_tests — test executable (Catch2)
  Sources: tests/test_smoke.cpp
  Links: matrix_lib
```

The C++ standard is set to C++17 project-wide. Compiler warnings (`-Wall -Wextra`) are applied to all targets. The build type defaults to `Debug` for development.

### 4.2 Catch2 integration

We use **Catch2 v3 single-header** (`catch_amalgamated.hpp` + `catch_amalgamated.cpp`) placed in `tests/catch2/`. This requires no `find_package`, no system install, and no internet access at build time — just two files checked into the repo.

The `matrix_tests` CMake target compiles Catch2 alongside our test files. `ctest` discovers and runs the test executable automatically.

### 4.3 CRLF fix

All `.cpp` and `.hpp` files are converted from CRLF to LF using a simple Python script (or `sed` / `dos2unix` where available). The fix is applied once; future edits should use LF-only editors or a `.gitattributes` rule.

We add a `.gitattributes` file to enforce LF for all C++ source files going forward:

```
*.cpp  text eol=lf
*.hpp  text eol=lf
*.md   text eol=lf
*.txt  text eol=lf
*.cmake text eol=lf
CMakeLists.txt text eol=lf
```

### 4.4 Directory layout after this plan

```
Matrix/
├── CMakeLists.txt                  ← NEW
├── .gitattributes                  ← NEW
├── AGENT_MD/
│   ├── spec.md
│   └── plan/
│       ├── rules.md
│       ├── current_state_report.md
│       └── plans/
│           └── PLAN-001_build_system_setup.md
├── include/
│   ├── Matrix.hpp
│   ├── operations/  (Add, Subtract, Multiply, Divide)
│   ├── solvers/     (LUDecomposition, SolveLinearEquation)
│   └── numerical/   (Eigen, EigenPower, Interpolation)
├── src/
│   ├── Matrix.cpp
│   ├── operations/  (Add, Subtract, Multiply, Divide)
│   ├── solvers/     (GaussElimination, GaussJacobi, GaussSeidel, LUDecomposition)
│   └── numerical/   (Eigen, EigenPower, Interpolation)
├── tests/
│   ├── CMakeLists.txt              ← NEW
│   ├── catch2/
│   │   ├── catch_amalgamated.hpp  ← NEW (Catch2 single-header)
│   │   └── catch_amalgamated.cpp  ← NEW (Catch2 single-source)
│   └── test_smoke.cpp              ← NEW
├── test_cases/
│   ├── matrix1.txt
│   ├── matrix2.txt
│   ├── matrix3.txt
│   └── matrix4.txt
└── main.cpp
```

---

## 5. Task Breakdown

| # | Task | Est. | Depends on |
|---|------|------|------------|
| 1 | Strip CRLF from all `.cpp` and `.hpp` files; add `.gitattributes` | 15 min | — |
| 2 | Write `CMakeLists.txt` (root): define `matrix_lib` and `matrix_app` targets, C++17, warnings | 30 min | 1 |
| 3 | Verify `matrix_app` builds and runs the menu without errors | 15 min | 2 |
| 4 | Download Catch2 v3 amalgamated files and place in `tests/catch2/` | 10 min | — |
| 5 | Write `tests/CMakeLists.txt`: define `matrix_tests` target, link `matrix_lib`, enable CTest | 20 min | 2, 4 |
| 6 | Write `tests/test_smoke.cpp`: 3 tests — Matrix constructor, `getRows()`, `getCols()` | 15 min | 4 |
| 7 | Run `ctest --test-dir build` — verify all 3 tests pass | 10 min | 5, 6 |
| 8 | Verify zero `\r` bytes remain in all source files | 5 min | 1 |
| 9 | Update `AGENT_MD/plan/current_state_report.md` with build system addition | 10 min | 7 |

**Total estimated time: ~2 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Catch2 amalgamated download unavailable | Low | Medium | Files can be generated locally from the Catch2 GitHub repo with `python amalgamate.py`; or use a manually copied v3 release |
| CMake `file(GLOB_RECURSE ...)` misses new `.cpp` files added later | Low | Low | Document that new source files must be added to `CMakeLists.txt` explicitly; or use GLOB with a note |
| CRLF stripping breaks a source file on Windows | Low | Low | Verify with `file` command before and after; keep a backup |
| Compiler warnings from existing code under `-Wall -Wextra` | Medium | Low | Start with `-Wall` only; promote to `-Wextra` once baseline is green; note any warnings in the report |

---

## 7. Success Criteria

- [ ] **G1 verified:** `cmake -B build && cmake --build build` exits 0, zero warnings in output
- [ ] **G2 verified:** `cmake --build build --target matrix_lib` and `--target matrix_app` both build independently
- [ ] **G3 verified:** `tests/test_smoke.cpp` exists with ≥ 3 `TEST_CASE` blocks
- [ ] **G4 verified:** `ctest --test-dir build --output-on-failure` exits 0, all tests shown as `Passed`
- [ ] **G5 verified:** `grep -rl $'\r' Matrix/src Matrix/include Matrix/main.cpp` returns empty output
- [ ] `main.cpp` menu still runs and responds to input correctly after build

---

## 8. References

- `AGENT_MD/spec.md` — Feature 0 (Pre-Work), Known Issues (no CMakeLists, CRLF)
- `AGENT_MD/plan/current_state_report.md` — §5 Infrastructure & Deployment
- Catch2 v3 release: https://github.com/catchorg/Catch2/releases
- CMake docs: https://cmake.org/cmake/help/latest/