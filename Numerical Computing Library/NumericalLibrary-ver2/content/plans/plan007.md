# PLAN-007: CLI / UX Improvements

**Created:** 2026-04-23
**Status:** Draft
**Addresses:** Feature 7 from specs.md — fix `main.cpp` indentation, modernise the menu to reflect all features added in PLANs 002–006, add convergence feedback for iterative solvers, and allow solver vector results to be saved to file.

---

## 1. Context & Motivation

`main.cpp` has not been touched since the project started. Every plan from 001–006 added new library features but explicitly deferred `main.cpp` changes to this plan. As a result, the menu is now significantly out of sync with the library:

| Issue | Detail |
|---|---|
| Menu option 6 says "Divide (2x2 only)" | Fixed in PLAN-003 — label is wrong |
| New solvers missing | Cholesky (PLAN-005) and QR (PLAN-005) have no menu entries |
| New methods missing | Newton interpolation (PLAN-006) and Inverse Power Method (PLAN-006) have no menu entries |
| New Matrix utilities missing | `transpose()`, `determinant()`, `inverse()` (all PLAN-002) have no menu entries |
| Jacobi/Seidel show no iteration count | PLAN-004 added convergence overloads — menu still uses old fixed-iteration API |
| Solver vector results not saveable | `printVector()` only prints to stdout; no file-save path |
| Inconsistent indentation | Cases 10–13 are misaligned relative to cases 1–9 |
| Local `isSquare()` free function | Redundant — `Matrix::isSquare()` added in PLAN-002 |
| No separator lines in menu | All 13+ options are a wall of text |

This plan fixes all of the above in a single pass.

---

## 2. Goals

- **G1:** Fix all indentation — cases 1–N uniform, helper functions consistent, clang-format compliant.
- **G2:** Update menu label: option 6 → "Divide (N×N)".
- **G3:** Add menu entries for all features added in PLANs 002–006 that are missing: `transpose`, `determinant`, `inverse`, Cholesky solve, QR solve, Newton interpolation, Inverse Power Method.
- **G4:** Jacobi and Seidel menu cases call the convergence-based overload (PLAN-004) and print "Converged in N iterations" or "Reached max iterations".
- **G5:** `printVector()` → `outputVector()`: add file-save path matching `outputResult()` (display / save / both).
- **G6:** Remove redundant local `isSquare()` free function — use `A.isSquare()` from the class.
- **G7:** Add separator lines between menu groups for readability.
- **G8:** `main.cpp` compiles cleanly with zero warnings under `-Wall -Wextra`.
- **G9:** All 81 existing tests continue to pass (no library code changes).

---

## 3. Non-Goals

- No algorithm changes — library code is untouched.
- No GUI or web interface — console-only.
- No input validation beyond what already exists (e.g. no retry loops on bad input).
- No new test cases — `main.cpp` is not unit-tested; manual smoke-test only.
- No CI pipeline changes.

---

## 4. Approach

### 4.1 Menu restructure

Group options into four logical sections with separator lines:

```
==== MATRIX OPERATIONS ====
 1. Load Matrix A
 2. Load Matrix B
 3. Add (A + B)
 4. Subtract (A - B)
 5. Multiply (A * B)
 6. Divide (A * inv(B))
 7. Transpose of A
 8. Determinant of A
 9. Inverse of A

==== LINEAR SOLVERS ====
10. Gauss Elimination
11. LU Decomposition
12. Cholesky Decomposition
13. QR Decomposition

==== ITERATIVE SOLVERS ====
14. Gauss-Jacobi
15. Gauss-Seidel

==== NUMERICAL METHODS ====
16. Lagrange Interpolation
17. Newton Interpolation
18. Gershgorin Eigenvalue Estimate
19. Power Method (dominant eigenvalue)
20. Inverse Power Method (smallest eigenvalue)

 0. Exit
```

### 4.2 Convergence output for iterative solvers

Cases 14 and 15 call the tolerance-based overload:

```cpp
case 14: {
    if (!A.isSquare()) { ... }
    auto b = inputVector(A.getRows());
    int iters = 0;
    auto x = GaussJacobi::solve(A, b, 1e-6, 1000, iters);
    std::cout << "Converged in " << iters << " iterations\n";
    outputVector(x);
    break;
}
```

### 4.3 outputVector() with file-save

```cpp
void outputVector(const std::vector<double>& x) {
    int choice;
    std::cout << "\nOutput options:\n1. Display\n2. Save to file\n3. Both\nChoice: ";
    std::cin >> choice;

    auto print = [&]() {
        std::cout << "Solution:\n";
        for (double v : x) std::cout << v << " ";
        std::cout << "\n";
    };

    auto save = [&]() {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;
        std::ofstream f(filename);
        for (double v : x) f << v << "\n";
        std::cout << "Saved to " << filename << "\n";
    };

    if (choice == 1) print();
    else if (choice == 2) save();
    else if (choice == 3) { print(); save(); }
    else std::cout << "Invalid choice\n";
}
```

### 4.4 Indentation standard

- 4-space indentation throughout.
- `switch` cases indented 4 spaces inside `switch`.
- Case body indented 4 spaces inside `case`.
- All `{` on same line as statement.
- Braces around all `case` bodies for scope isolation.

---

## 5. Task Breakdown

| # | Task | Est. |
|---|------|------|
| 1 | Remove local `isSquare()` free function; replace all uses with `A.isSquare()` | 5 min |
| 2 | Fix indentation of cases 10–13 to match 1–9 | 10 min |
| 3 | Update menu label for option 6 | 2 min |
| 4 | Add separator lines and group headers to menu | 10 min |
| 5 | Add missing menu entries (7–9: transpose, determinant, inverse) with implementations | 20 min |
| 6 | Renumber remaining cases to fit new menu layout | 15 min |
| 7 | Add Cholesky and QR solver menu cases (new cases 12, 13) | 20 min |
| 8 | Update Jacobi and Seidel cases to use convergence overload; print iters | 15 min |
| 9 | Replace `printVector()` with `outputVector()` (adds file-save path) | 20 min |
| 10 | Add Newton interpolation menu case | 10 min |
| 11 | Add Inverse Power Method menu case | 10 min |
| 12 | Add missing includes (`Cholesky.hpp`, `QR.hpp`, `<fstream>`) | 5 min |
| 13 | Build and verify zero warnings; manual smoke test of all options | 15 min |

**Total estimated time: ~2.5 hours**

---

## 6. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Renumbering cases breaks user muscle memory | Low | Low | Acceptable — menu is self-describing; numbers are shown at runtime |
| `outputVector()` with `<fstream>` include not present | Low | Low | Add `#include <fstream>` at top of file |
| Cholesky/QR headers not included | Low | Low | Add `#include "solvers/Cholesky.hpp"` and `#include "solvers/QR.hpp"` |
| Convergence overload signature mismatch | Low | Low | Verify against `SolveLinearEquation.hpp` before coding |

---

## 7. Success Criteria

- [ ] **G1:** `main.cpp` has 4-space uniform indentation throughout
- [ ] **G2:** Menu option for divide reads "Divide (A * inv(B))"
- [ ] **G3:** All 20 menu options present and functional
- [ ] **G4:** Jacobi/Seidel print "Converged in N iterations"
- [ ] **G5:** Solver vector output offers display / save / both
- [ ] **G6:** Local `isSquare()` removed; `A.isSquare()` used
- [ ] **G7:** Menu has group separator lines
- [ ] **G8:** `g++ -std=c++17 -Wall -Wextra main.cpp ...` → zero warnings
- [ ] **G9:** `ctest` still shows 81/81 passing (no lib changes)

---

## 8. References

- `specs.md` — Feature 7 (CLI / UX Improvements), Success Criteria v2.0
- `report006.md` — last completed plan; 81 tests passing
- `library/MatrixV1/main.cpp` — file to be rewritten
- `library/MatrixV1/include/solvers/SolveLinearEquation.hpp` — convergence overload signatures
- `library/MatrixV1/include/solvers/Cholesky.hpp` — to be added to menu
- `library/MatrixV1/include/solvers/QR.hpp` — to be added to menu