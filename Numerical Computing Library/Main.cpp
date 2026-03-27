#include "RootFinding/include/RootFinding.hpp"
#include "ComplexNumber/include/ComplexNumber.hpp"
#include "Matrix/include/Matrix.hpp"

#include <iostream>
#include <cmath>
#include <iomanip>

using namespace Numerical;

// ─── helpers ─────────────────────────────────────────────────────────────────
static void banner(const std::string& title) {
    std::cout << "\n╔══════════════════════════════════════════════╗\n";
    std::cout <<   "║  " << std::left << std::setw(44) << title << "║\n";
    std::cout <<   "╚══════════════════════════════════════════════╝\n";
}

// ─── Root-Finding demo ────────────────────────────────────────────────────────
void demoRootFinding() {
    banner("ROOT-FINDING  —  f(x) = x³ - x - 2");

    auto f  = [](double x){ return x*x*x - x - 2.0; };
    auto df = [](double x){ return 3.0*x*x - 1.0; };
    auto d2f= [](double x){ return 6.0*x; };
    auto g  = [](double x){ return std::cbrt(x + 2.0); }; // fixed-point form

    RootFinder rf(1e-12, 500);

    RootFinder::printResult(rf.bisection   (f,       1.0, 2.0));
    RootFinder::printResult(rf.regulaFalsi (f,       1.0, 2.0));
    RootFinder::printResult(rf.illinois    (f,       1.0, 2.0));
    RootFinder::printResult(rf.brent       (f,       1.0, 2.0));
    RootFinder::printResult(rf.newton      (f, df,   1.5));
    RootFinder::printResult(rf.halley      (f, df, d2f, 1.5));
    RootFinder::printResult(rf.secant      (f,       1.0, 2.0));
    RootFinder::printResult(rf.fixedPoint  (g,       1.5));
    RootFinder::printResult(rf.steffensen  (g,       1.5));
    RootFinder::printResult(rf.ridder      (f,       1.0, 2.0));
}

// ─── Complex Numbers demo ─────────────────────────────────────────────────────
void demoComplexNumbers() {
    banner("COMPLEX NUMBERS");

    Complex a(3.0, 4.0);
    Complex b(1.0, -2.0);

    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n\n";

    std::cout << "a + b      = " << (a + b)          << "\n";
    std::cout << "a - b      = " << (a - b)          << "\n";
    std::cout << "a * b      = " << (a * b)          << "\n";
    std::cout << "a / b      = " << (a / b)          << "\n\n";

    std::cout << "|a|        = " << a.modulus()      << "\n";
    std::cout << "arg(a)     = " << a.argument()     << " rad\n";
    std::cout << "conj(a)    = " << a.conjugate()    << "\n";
    std::cout << "1/a        = " << a.inverse()      << "\n";
    std::cout << "a²         = " << a.pow(2)         << "\n";
    std::cout << "√a         = " << a.sqrt()         << "\n";
    std::cout << "ln(a)      = " << a.log()          << "\n";
    std::cout << "exp(a)     = " << a.exp()          << "\n\n";

    std::cout << "sin(a)     = " << a.sin()          << "\n";
    std::cout << "cos(a)     = " << a.cos()          << "\n";
    std::cout << "sinh(a)    = " << a.sinh()         << "\n";
    std::cout << "cosh(a)    = " << a.cosh()         << "\n\n";

    // Euler's identity: e^{iπ} + 1 = 0
    Complex euler = (Complex::I * M_PI).exp() + Complex::ONE;
    std::cout << "e^(iπ) + 1 = " << euler            << "  (≈ 0, Euler's identity)\n";

    // Polar
    auto [r, theta] = a.toPolar();
    std::cout << "\nPolar:  r=" << r << ", θ=" << theta << " rad\n";
    std::cout << "Verify: " << Complex::polar(r, theta) << "\n";

    // All 4th roots of a
    auto roots = a.nthRoots(4);
    std::cout << "\n4th roots of a:\n";
    for (int i = 0; i < (int)roots.size(); ++i)
        std::cout << "  root[" << i << "] = " << roots[i] << "\n";
}

// ─── Matrix demo ──────────────────────────────────────────────────────────────
void demoMatrix() {
    banner("MATRIX OPERATIONS");

    // ─── Basic ops ────────────────────────────────────────────────────────────
    Matrix A{{4, 3, 2, 1},
             {3, 4, 3, 2},
             {2, 3, 4, 3},
             {1, 2, 3, 4}};

    Matrix b{{1},{2},{3},{4}};

    A.print(3, "\nMatrix A:");
    b.print(3, "\nRHS b:");

    std::cout << "\nTrace(A) = "      << A.trace()      << "\n";
    std::cout << "det(A)   = "        << A.determinant()<< "\n";
    std::cout << "rank(A)  = "        << A.rank()       << "\n";
    std::cout << "Frobenius norm = "  << A.frobenius()  << "\n";
    std::cout << "Symmetric? "        << (A.isSymmetric() ? "Yes" : "No") << "\n";

    // ─── LU Decomposition ────────────────────────────────────────────────────
    std::cout << "\n--- LU Decomposition (P·A = L·U) ---\n";
    auto [L, U, P] = A.luDecomposition();
    L.print(3, "L:"); U.print(3, "U:"); P.print(0, "P:");

    // ─── Solve A·x = b ────────────────────────────────────────────────────────
    Matrix x = A.solve(b);
    x.print(6, "\nSolution x (LU solve):");

    // Verify A·x ≈ b
    Matrix residual = A * x - b;
    std::cout << "Residual ‖Ax−b‖ = " << residual.frobenius() << "\n";

    // ─── Gaussian / Gauss-Jordan ─────────────────────────────────────────────
    A.gaussianElimination(b).print(6, "\nSolution (Gaussian Elimination):");
    A.gaussJordan(b).print(6, "\nSolution (Gauss-Jordan):");

    // ─── Cholesky (SPD matrix) ────────────────────────────────────────────────
    Matrix Spd{{6,2,1},{2,5,2},{1,2,4}};
    Matrix bs{{1},{2},{3}};
    std::cout << "\n--- Cholesky (SPD 3×3) ---\n";
    Spd.choleskyDecomposition().print(4, "L (Cholesky):");
    Spd.solveSPD(bs).print(6, "Solution (Cholesky solve):");

    // ─── QR Decomposition ────────────────────────────────────────────────────
    std::cout << "\n--- QR Decomposition ---\n";
    auto [Q, R] = A.qrDecomposition();
    Q.print(4, "Q:"); R.print(4, "R:");

    // ─── Least Squares ────────────────────────────────────────────────────────
    Matrix Aover{{1,1},{1,2},{1,3},{1,4}};
    Matrix bover{{2},{3.9},{6.1},{8}};
    Aover.solveLeastSquares(bover).print(6, "\nLeast-Squares fit [1,x] to {2,3.9,6.1,8}:");

    // ─── Eigenvalues ──────────────────────────────────────────────────────────
    Matrix Sym3{{2,1,0},{1,2,1},{0,1,2}};
    std::cout << "\n--- Eigenvalues (QR method) ---\n";
    auto eigs = Sym3.eigenvaluesQR(1e-12, 500);
    std::cout << "Eigenvalues: ";
    for (double ev : eigs) std::cout << std::fixed << std::setprecision(6) << ev << "  ";
    std::cout << "\n";

    auto [eigVals, eigVecs] = Sym3.jacobiEigen();
    std::cout << "\nEigenvalues (Jacobi): ";
    for (double ev : eigVals) std::cout << ev << "  ";
    eigVecs.print(4, "\nEigenvectors (columns):");

    // ─── Iterative solvers ────────────────────────────────────────────────────
    Matrix Aiter{{4,1,0},{1,3,1},{0,1,4}};
    Matrix biter{{9},{10},{9}};
    Aiter.jacobiIteration(biter).print(6, "\nJacobi Iteration solution:");
    Aiter.gaussSeidelIteration(biter).print(6, "Gauss-Seidel solution:");
    Aiter.sor(biter, 1.2).print(6, "SOR (ω=1.2) solution:");
    Aiter.conjugateGradient(biter).print(6, "Conjugate Gradient solution:");

    // ─── Inverse & Pseudo-inverse ─────────────────────────────────────────────
    A.inverse().print(6, "\nA⁻¹:");
    (A * A.inverse()).print(2, "A · A⁻¹ (≈ I):");

    // ─── Thomas Algorithm (tridiagonal) ──────────────────────────────────────
    std::vector<double> lo  = {1, 1, 1};
    std::vector<double> main= {4, 4, 4, 4};
    std::vector<double> up  = {1, 1, 1};
    std::vector<double> rhs = {5, 6, 6, 5};
    auto sol = Matrix::thomasAlgorithm(lo, main, up, rhs);
    std::cout << "\nThomas Algorithm (tridiagonal) solution:\n[ ";
    for (double v : sol) std::cout << std::fixed << std::setprecision(6) << v << "  ";
    std::cout << "]\n";
}

// ─── Entry point ──────────────────────────────────────────────────────────────
int main() {
    demoRootFinding();
    demoComplexNumbers();
    demoMatrix();
    std::cout << "\nAll demos completed.\n";
    return 0;
}