#include "../../include/numerical/LeastSquares.hpp"
#include "../../include/Matrix.hpp"
#include "../../include/solvers/SolveLinearEquation.hpp"
#include <stdexcept>

// ─────────────────────────────────────────────────────────────────────────────
// fitLine — best-fit straight line  y = a + b*x
//
// Normal equations (2×2 system derived by setting ∂S/∂a = ∂S/∂b = 0):
//
//   [ n     Σx  ] [a]   [Σy  ]
//   [ Σx    Σx² ] [b] = [Σxy ]
//
// Solved by GaussElimination::solve().
// ─────────────────────────────────────────────────────────────────────────────
std::pair<double, double>
LeastSquares::fitLine(const std::vector<double>& x,
                      const std::vector<double>& y) {
    int n = static_cast<int>(x.size());

    if (n == 0 || static_cast<int>(y.size()) != n) {
        throw std::invalid_argument(
            "LeastSquares::fitLine: x and y must be non-empty and the same size");
    }
    if (n < 2) {
        throw std::invalid_argument(
            "LeastSquares::fitLine: at least 2 data points are required");
    }

    // Single-pass accumulation of required sums
    double sx = 0.0, sx2 = 0.0, sy = 0.0, sxy = 0.0;
    for (int i = 0; i < n; i++) {
        sx  += x[i];
        sx2 += x[i] * x[i];
        sy  += y[i];
        sxy += x[i] * y[i];
    }

    // Build 2×2 normal-equation matrix
    Matrix A(2, 2);
    A.set(0, 0, static_cast<double>(n));  A.set(0, 1, sx);
    A.set(1, 0, sx);                      A.set(1, 1, sx2);

    std::vector<double> rhs = {sy, sxy};

    // Solve; GaussElimination throws std::runtime_error on singular matrix.
    // Catch and rethrow with a more descriptive message.
    std::vector<double> coeffs;
    try {
        coeffs = GaussElimination::solve(A, rhs);
    } catch (const std::runtime_error&) {
        throw std::invalid_argument(
            "LeastSquares::fitLine: normal-equation matrix is singular "
            "(all x values may be identical)");
    }

    return {coeffs[0], coeffs[1]};
}

// ─────────────────────────────────────────────────────────────────────────────
// fitParabola — best-fit parabola  y = a + b*x + c*x²
//
// Normal equations (3×3 system derived by setting ∂S/∂a = ∂S/∂b = ∂S/∂c = 0):
//
//   [ n     Σx    Σx²  ] [a]   [Σy   ]
//   [ Σx    Σx²   Σx³  ] [b] = [Σxy  ]
//   [ Σx²   Σx³   Σx⁴  ] [c]   [Σx²y ]
//
// Solved by GaussElimination::solve().
// ─────────────────────────────────────────────────────────────────────────────
std::tuple<double, double, double>
LeastSquares::fitParabola(const std::vector<double>& x,
                          const std::vector<double>& y) {
    int n = static_cast<int>(x.size());

    if (n == 0 || static_cast<int>(y.size()) != n) {
        throw std::invalid_argument(
            "LeastSquares::fitParabola: x and y must be non-empty and the same size");
    }
    if (n < 3) {
        throw std::invalid_argument(
            "LeastSquares::fitParabola: at least 3 data points are required");
    }

    // Single-pass accumulation of required sums
    double sx = 0.0, sx2 = 0.0, sx3 = 0.0, sx4 = 0.0;
    double sy = 0.0, sxy = 0.0, sx2y = 0.0;

    for (int i = 0; i < n; i++) {
        double xi  = x[i];
        double xi2 = xi * xi;
        double xi3 = xi2 * xi;
        double xi4 = xi2 * xi2;

        sx   += xi;
        sx2  += xi2;
        sx3  += xi3;
        sx4  += xi4;
        sy   += y[i];
        sxy  += xi  * y[i];
        sx2y += xi2 * y[i];
    }

    // Build 3×3 normal-equation matrix
    Matrix A(3, 3);
    A.set(0, 0, static_cast<double>(n)); A.set(0, 1, sx);  A.set(0, 2, sx2);
    A.set(1, 0, sx);                     A.set(1, 1, sx2); A.set(1, 2, sx3);
    A.set(2, 0, sx2);                    A.set(2, 1, sx3); A.set(2, 2, sx4);

    std::vector<double> rhs = {sy, sxy, sx2y};

    std::vector<double> coeffs;
    try {
        coeffs = GaussElimination::solve(A, rhs);
    } catch (const std::runtime_error&) {
        throw std::invalid_argument(
            "LeastSquares::fitParabola: normal-equation matrix is singular "
            "(degenerate x distribution)");
    }

    return {coeffs[0], coeffs[1], coeffs[2]};
}
