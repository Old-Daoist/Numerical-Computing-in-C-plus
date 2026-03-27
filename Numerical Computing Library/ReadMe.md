# NumericalComputing — C++17 Library

A header+source C++ library covering three core areas of numerical computing.

---

## Folder Structure

```
NumericalComputing/
├── CMakeLists.txt
├── main.cpp                        ← Demo / test driver
│
├── RootFinding/
│   ├── include/RootFinding.h
│   └── src/RootFinding.cpp
│
├── ComplexNumbers/
│   ├── include/ComplexNumbers.h
│   └── src/ComplexNumbers.cpp
│
└── Matrix/
    ├── include/Matrix.h
    └── src/Matrix.cpp
```

---

## Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
./demo
```

---

## Modules

### 1 · RootFinding  (`Numerical::RootFinder`)

All methods solve **f(x) = 0** and return a `RootResult` with root, residual, iterations and convergence flag.

| Method | Class | Convergence |
|---|---|---|
| `bisection` | Bracketing | Linear (1/2ⁿ) |
| `regulaFalsi` | Bracketing | Super-linear |
| `illinois` | Bracketing | Super-linear (improved RF) |
| `brent` | Bracketing | Near-optimal (guaranteed) |
| `newton` | Open | Quadratic |
| `halley` | Open | Cubic |
| `secant` | Open | ~1.618 |
| `fixedPoint` | Open | Linear (if |g'|<1) |
| `steffensen` | Open | Quadratic (no f' needed) |
| `ridder` | Bracketing | Super-linear |

```cpp
RootFinder rf(1e-12, 500);
auto f  = [](double x){ return x*x - 2; };
auto df = [](double x){ return 2*x; };
auto r  = rf.newton(f, df, 1.5);
// r.root ≈ 1.41421356237
```

---

### 2 · ComplexNumbers  (`Numerical::Complex`)

Full operator overloading, polar / exponential construction, and transcendental functions.

| Category | Methods |
|---|---|
| Constructors | Cartesian, `polar(r,θ)`, `fromExp(a,b)` |
| Arithmetic | `+  -  *  /` (complex & scalar), unary `-` |
| Properties | `modulus`, `modulusSq`, `argument`, `conjugate`, `inverse`, `normalize` |
| Predicates | `isReal`, `isPureImag`, `isZero` |
| Algebra | `pow(int)`, `pow(Complex)`, `sqrt`, `log`, `log(base)`, `exp` |
| Trig | `sin cos tan cot sec csc` |
| Hyperbolic | `sinh cosh tanh` |
| Inverse trig | `asin acos atan` |
| Roots | `nthRoots(n)` — returns all n roots |
| Representation | `toPolar()`, `toString()`, `<<`, `>>` |
| Constants | `Complex::ZERO ONE I NEG_ONE` |

```cpp
Complex z(3, 4);
std::cout << z.modulus();   // 5
std::cout << z.sqrt();      // 2 + 1i
auto roots = z.nthRoots(3); // three cube roots
```

---

### 3 · Matrix  (`Numerical::Matrix`)

Dense matrix with direct solvers, iterative solvers, decompositions and eigenvalue methods.

#### Decompositions
| Method | Returns | Notes |
|---|---|---|
| `luDecomposition()` | {L, U, P} | Partial pivoting |
| `ldltDecomposition()` | {L, D} | Symmetric matrices |
| `choleskyDecomposition()` | L | SPD matrices |
| `qrDecomposition()` | {Q, R} | Householder reflections |
| `svd()` | {U, Σ, Vᵀ} | Via Jacobi on AᵀA |
| `hessenbergReduction()` | {Q, H} | Householder |
| `schurDecomposition()` | {Q, T} | QR iteration |

#### Direct Solvers
| Method | Algorithm |
|---|---|
| `solve(b)` | LU with partial pivoting |
| `solveSPD(b)` | Cholesky |
| `solveLeastSquares(b)` | QR |
| `gaussianElimination(b)` | Partial-pivot GE |
| `gaussJordan(b)` | Reduced row echelon |
| `thomasAlgorithm(lo,d,up,rhs)` | Tridiagonal O(n) |

#### Iterative Solvers
| Method | Description |
|---|---|
| `jacobiIteration` | Classic Jacobi |
| `gaussSeidelIteration` | Forward sweep |
| `sor(b, omega)` | Successive Over-Relaxation |
| `conjugateGradient` | SPD — optimal Krylov |

#### Eigenvalue Methods
| Method | Description |
|---|---|
| `powerIteration` | Dominant eigenvalue/vector |
| `inverseIteration(mu)` | Eigenvalue nearest μ |
| `eigenvaluesQR` | All eigenvalues (shifted QR) |
| `jacobiEigen` | All eigenvalues+vectors (symmetric) |

#### Utilities
`determinant · inverse · rank · nullity · pseudoInverse · conditionNumber · transpose · trace · frobenius · norm1 · normInf`

#### Properties
`isSymmetric · isOrthogonal · isDiagonallyDominant · isPositiveDefinite · isSingular`

---

## Namespace

All classes live in `namespace Numerical`. Include the relevant header and link against the corresponding static library (or compile the `.cpp` directly).

---

## Requirements

- C++17 or newer
- CMake ≥ 3.16 (for the provided build system)
- Standard library only — no external dependencies