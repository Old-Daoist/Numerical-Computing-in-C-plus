#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Matrix.hpp"
#include "operations/Add.hpp"
#include "operations/Subtract.hpp"
#include "operations/Multiply.hpp"
#include "operations/Divide.hpp"

#include "solvers/SolveLinearEquation.hpp"
#include "solvers/LUDecomposition.hpp"
#include "solvers/Cholesky.hpp"
#include "solvers/QR.hpp"

#include "numerical/Interpolation.hpp"
#include "numerical/Eigen.hpp"
#include "numerical/EigenPower.hpp"
#include "numerical/LeastSquares.hpp"

// ─── Matrix input ─────────────────────────────────────────────────────────────

void loadMatrix(Matrix& M) {
    int choice;
    std::cout << "\n  1. Input manually\n  2. Input from file\n  Choice: ";
    std::cin >> choice;

    if (choice == 1) {
        int r, c;
        std::cout << "  Enter rows and columns: ";
        std::cin >> r >> c;
        M = Matrix(r, c);
        M.input();
    } else {
        std::string filename;
        std::cout << "  Enter filename: ";
        std::cin >> filename;
        M.inputFromFile(filename);
        std::cout << "  Loaded from: " << filename << "\n";
    }
}

// ─── Matrix output ────────────────────────────────────────────────────────────

void outputResult(const Matrix& result) {
    int choice;
    std::cout << "\n  Output options:\n";
    std::cout << "  1. Display on terminal\n";
    std::cout << "  2. Save to file\n";
    std::cout << "  3. Both\n";
    std::cout << "  Choice: ";
    std::cin >> choice;

    if (choice == 1) {
        result.display();
    } else if (choice == 2) {
        std::string filename;
        std::cout << "  Enter filename: ";
        std::cin >> filename;
        result.saveToFile(filename);
        std::cout << "  Saved to " << filename << "\n";
    } else if (choice == 3) {
        std::string filename;
        std::cout << "  Enter filename: ";
        std::cin >> filename;
        result.display();
        result.saveToFile(filename);
        std::cout << "  Also saved to " << filename << "\n";
    } else {
        std::cout << "  Invalid choice\n";
    }
}

// ─── Vector input / output ───────────────────────────────────────────────────

std::vector<double> inputVector(int n) {
    std::vector<double> b(n);
    std::cout << "  Enter RHS vector (" << n << " values): ";
    for (int i = 0; i < n; i++) {
        std::cin >> b[i];
    }
    return b;
}

void outputVector(const std::vector<double>& x) {
    int choice;
    std::cout << "\n  Output options:\n";
    std::cout << "  1. Display on terminal\n";
    std::cout << "  2. Save to file\n";
    std::cout << "  3. Both\n";
    std::cout << "  Choice: ";
    std::cin >> choice;

    auto print = [&]() {
        std::cout << "  Solution: ";
        for (double v : x) {
            std::cout << v << "  ";
        }
        std::cout << "\n";
    };

    auto save = [&]() {
        std::string filename;
        std::cout << "  Enter filename: ";
        std::cin >> filename;
        std::ofstream f(filename);
        for (double v : x) {
            f << v << "\n";
        }
        std::cout << "  Saved to " << filename << "\n";
    };

    if (choice == 1) {
        print();
    } else if (choice == 2) {
        save();
    } else if (choice == 3) {
        print();
        save();
    } else {
        std::cout << "  Invalid choice\n";
    }
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    Matrix A, B;
    int choice;

    while (true) {
        std::cout << "\n";
        std::cout << "  ╔══════════════════════════════════╗\n";
        std::cout << "  ║        MATRIX OPERATIONS         ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║  1.  Load Matrix A               ║\n";
        std::cout << "  ║  2.  Load Matrix B               ║\n";
        std::cout << "  ║  3.  Add          (A + B)        ║\n";
        std::cout << "  ║  4.  Subtract     (A - B)        ║\n";
        std::cout << "  ║  5.  Multiply     (A * B)        ║\n";
        std::cout << "  ║  6.  Divide       (A * inv(B))   ║\n";
        std::cout << "  ║  7.  Transpose of A              ║\n";
        std::cout << "  ║  8.  Determinant of A            ║\n";
        std::cout << "  ║  9.  Inverse of A                ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║        LINEAR SOLVERS            ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║  10. Gauss Elimination           ║\n";
        std::cout << "  ║  11. LU Decomposition            ║\n";
        std::cout << "  ║  12. Cholesky Decomposition      ║\n";
        std::cout << "  ║  13. QR Decomposition            ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║        ITERATIVE SOLVERS         ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║  14. Gauss-Jacobi                ║\n";
        std::cout << "  ║  15. Gauss-Seidel                ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║        NUMERICAL METHODS         ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║  16. Lagrange Interpolation      ║\n";
        std::cout << "  ║  17. Newton Interpolation        ║\n";
        std::cout << "  ║  18. Gershgorin Eigenvalue Est.  ║\n";
        std::cout << "  ║  19. Power Method (dominant λ)   ║\n";
        std::cout << "  ║  20. Inverse Power (smallest λ)  ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║        CURVE FITTING             ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║  21. Least Squares — Line        ║\n";
        std::cout << "  ║  22. Least Squares — Parabola    ║\n";
        std::cout << "  ╠══════════════════════════════════╣\n";
        std::cout << "  ║   0. Exit                        ║\n";
        std::cout << "  ╚══════════════════════════════════╝\n";
        std::cout << "  Choice: ";
        std::cin >> choice;

        try {
            switch (choice) {

                // ── Load ──────────────────────────────────────────────────────
                case 1: {
                    loadMatrix(A);
                    break;
                }

                case 2: {
                    loadMatrix(B);
                    break;
                }

                // ── Arithmetic ────────────────────────────────────────────────
                case 3: {
                    Matrix result = A + B;
                    outputResult(result);
                    break;
                }

                case 4: {
                    Matrix result = A - B;
                    outputResult(result);
                    break;
                }

                case 5: {
                    Matrix result = A * B;
                    outputResult(result);
                    break;
                }

                case 6: {
                    Matrix result = Divide::compute(A, B);
                    outputResult(result);
                    break;
                }

                // ── Matrix utilities ──────────────────────────────────────────
                case 7: {
                    Matrix result = A.transpose();
                    outputResult(result);
                    break;
                }

                case 8: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    double det = A.determinant();
                    std::cout << "  Determinant of A = " << det << "\n";
                    break;
                }

                case 9: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    Matrix result = A.inverse();
                    outputResult(result);
                    break;
                }

                // ── Linear solvers ────────────────────────────────────────────
                case 10: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    auto b = inputVector(A.getRows());
                    auto x = GaussElimination::solve(A, b);
                    outputVector(x);
                    break;
                }

                case 11: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    auto b = inputVector(A.getRows());
                    auto x = LU::solve(A, b);
                    outputVector(x);
                    break;
                }

                case 12: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    auto b = inputVector(A.getRows());
                    auto x = Cholesky::solve(A, b);
                    outputVector(x);
                    break;
                }

                case 13: {
                    auto b = inputVector(A.getRows());
                    auto x = QR::solve(A, b);
                    outputVector(x);
                    break;
                }

                // ── Iterative solvers ─────────────────────────────────────────
                case 14: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    auto b = inputVector(A.getRows());
                    int iters = 0;
                    auto x = GaussJacobi::solve(A, b, 1e-6, 1000, iters);
                    std::cout << "  Converged in " << iters << " iteration(s).\n";
                    outputVector(x);
                    break;
                }

                case 15: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    auto b = inputVector(A.getRows());
                    int iters = 0;
                    auto x = GaussSeidel::solve(A, b, 1e-6, 1000, iters);
                    std::cout << "  Converged in " << iters << " iteration(s).\n";
                    outputVector(x);
                    break;
                }

                // ── Numerical methods ─────────────────────────────────────────
                case 16: {
                    int n;
                    std::cout << "  Number of data points: ";
                    std::cin >> n;
                    std::vector<double> xs(n), ys(n);
                    std::cout << "  Enter x values: ";
                    for (int i = 0; i < n; i++) { std::cin >> xs[i]; }
                    std::cout << "  Enter y values: ";
                    for (int i = 0; i < n; i++) { std::cin >> ys[i]; }
                    double val;
                    std::cout << "  Interpolate at x = ";
                    std::cin >> val;
                    std::cout << "  Lagrange result = "
                              << Interpolation::lagrange(xs, ys, val) << "\n";
                    break;
                }

                case 17: {
                    int n;
                    std::cout << "  Number of data points: ";
                    std::cin >> n;
                    std::vector<double> xs(n), ys(n);
                    std::cout << "  Enter x values: ";
                    for (int i = 0; i < n; i++) { std::cin >> xs[i]; }
                    std::cout << "  Enter y values: ";
                    for (int i = 0; i < n; i++) { std::cin >> ys[i]; }
                    double val;
                    std::cout << "  Interpolate at x = ";
                    std::cin >> val;
                    std::cout << "  Newton result = "
                              << Interpolation::newton(xs, ys, val) << "\n";
                    break;
                }

                case 18: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    Eigen::gershgorin(A);
                    break;
                }

                case 19: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    std::vector<double> eigenvector;
                    double eigenvalue = EigenPower::powerMethod(A, eigenvector);
                    std::cout << "  Dominant eigenvalue  = " << eigenvalue << "\n";
                    std::cout << "  Eigenvector: ";
                    for (double v : eigenvector) { std::cout << v << "  "; }
                    std::cout << "\n";
                    break;
                }

                case 20: {
                    if (!A.isSquare()) {
                        std::cout << "  Matrix A must be square.\n";
                        break;
                    }
                    std::vector<double> eigenvector;
                    int iters = 0;
                    double eigenvalue = EigenPower::inverseMethod(
                        A, eigenvector, 1e-9, 1000, iters);
                    std::cout << "  Smallest eigenvalue  = " << eigenvalue << "\n";
                    std::cout << "  Converged in " << iters << " iteration(s).\n";
                    std::cout << "  Eigenvector: ";
                    for (double v : eigenvector) { std::cout << v << "  "; }
                    std::cout << "\n";
                    break;
                }

                // ── Curve fitting ─────────────────────────────────────────────
                case 21: {
                    int n;
                    std::cout << "  Number of data points: ";
                    std::cin >> n;
                    std::vector<double> xs(n), ys(n);
                    std::cout << "  Enter x values: ";
                    for (int i = 0; i < n; i++) { std::cin >> xs[i]; }
                    std::cout << "  Enter y values: ";
                    for (int i = 0; i < n; i++) { std::cin >> ys[i]; }
                    auto [a, b] = LeastSquares::fitLine(xs, ys);
                    std::cout << "  Fitted line:  y = " << a
                              << " + " << b << "x\n";
                    break;
                }

                case 22: {
                    int n;
                    std::cout << "  Number of data points: ";
                    std::cin >> n;
                    std::vector<double> xs(n), ys(n);
                    std::cout << "  Enter x values: ";
                    for (int i = 0; i < n; i++) { std::cin >> xs[i]; }
                    std::cout << "  Enter y values: ";
                    for (int i = 0; i < n; i++) { std::cin >> ys[i]; }
                    auto [a, b, c] = LeastSquares::fitParabola(xs, ys);
                    std::cout << "  Fitted curve: y = " << a
                              << " + " << b << "x"
                              << " + " << c << "x\xc2\xb2\n";
                    break;
                }

                // ── Exit ──────────────────────────────────────────────────────
                case 0: {
                    std::cout << "  Goodbye.\n";
                    return 0;
                }

                default: {
                    std::cout << "  Invalid choice — enter a number from the menu.\n";
                    break;
                }
            }

        } catch (const std::exception& e) {
            std::cout << "  Error: " << e.what() << "\n";
        }
    }
}