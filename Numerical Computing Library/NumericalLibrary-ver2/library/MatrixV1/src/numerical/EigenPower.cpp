#include "../../include/numerical/EigenPower.hpp"
#include "../../include/solvers/LUDecomposition.hpp"
#include <cmath>
#include <stdexcept>

// ─── Power Method (original — unchanged) ─────────────────────────────────────

double EigenPower::powerMethod(const Matrix& A,
                               std::vector<double>& eigenvector,
                               int iterations) {

    int n = A.getRows();

    if(A.getRows() != A.getCols())
        throw std::runtime_error("Matrix must be square");

    eigenvector.assign(n, 1.0);

    double eigenvalue = 0;

    for(int it = 0; it < iterations; it++) {
        std::vector<double> temp(n, 0);

        // Multiply A * x
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                temp[i] += A.get(i,j) * eigenvector[j];
            }
        }

        // Find max element (normalization)
        double maxVal = std::abs(temp[0]);
        for(int i = 1; i < n; i++) {
            if(std::abs(temp[i]) > maxVal)
                maxVal = std::abs(temp[i]);
        }

        if(maxVal < 1e-12)
            throw std::runtime_error("Power method failed (zero vector)");

        // Normalize
        for(int i = 0; i < n; i++) {
            eigenvector[i] = temp[i] / maxVal;
        }

        eigenvalue = maxVal;
    }

    return eigenvalue;
}

// ─── Inverse Power Method — internal implementation ──────────────────────────
//
// The inverse power method finds the eigenvalue of A closest to 0 (i.e. the
// smallest magnitude eigenvalue).
//
// Key insight: if A has eigenvalue λ, then A⁻¹ has eigenvalue 1/λ.
// Applying the power method to A⁻¹ converges to its dominant eigenvalue,
// which is 1/λ_min — so λ_min = 1 / (dominant eigenvalue of A⁻¹).
//
// Instead of explicitly computing A⁻¹, each iteration solves A*w = v via LU,
// which is equivalent to multiplying v by A⁻¹.
//
// Convergence criterion: ||v_new - v_old||_2 < tol (same as PLAN-004 iterative solvers).
//
static double inverseMethodImpl(const Matrix& A,
                                std::vector<double>& eigenvector,
                                double tol,
                                int maxIter,
                                int& iters) {
    if (!A.isSquare()) {
        throw std::runtime_error("Inverse power method: matrix must be square");
    }

    int n = A.getRows();

    // Initial guess: asymmetric vector (e.g. 1.0, 2.0, 3.0...) to avoid orthogonality
    // to any standard eigenvector (unlike all-ones or basis vectors).
    eigenvector.assign(n, 0.0);
    double norm = 0.0;
    for (int i = 0; i < n; i++) {
        eigenvector[i] = static_cast<double>(i + 1);
        norm += eigenvector[i] * eigenvector[i];
    }
    norm = std::sqrt(norm);
    for (int i = 0; i < n; i++) {
        eigenvector[i] /= norm;
    }

    double eigenvalue = 0.0;
    iters = 0;

    for (int k = 0; k < maxIter; k++) {
        // Solve A * w = v  (one step of inverse iteration)
        std::vector<double> w;
        try {
            w = LU::solve(A, eigenvector);
        } catch (const std::exception& e) {
            throw std::runtime_error(
                std::string("Inverse power method: LU solve failed — matrix may be singular: ") +
                e.what());
        }

        // Infinity-norm (max-abs) of w → used to extract eigenvalue estimate
        double maxAbsW = std::abs(w[0]);
        for (int i = 1; i < n; i++) {
            if (std::abs(w[i]) > maxAbsW)
                maxAbsW = std::abs(w[i]);
        }

        if (maxAbsW < 1e-12) {
            throw std::runtime_error("Inverse power method: zero vector encountered");
        }

        // Eigenvalue of A (smallest) = 1 / ||w||_inf
        eigenvalue = 1.0 / maxAbsW;

        // Normalise w → new eigenvector estimate
        std::vector<double> v_new(n);
        for (int i = 0; i < n; i++) {
            v_new[i] = w[i] / maxAbsW;
        }

        // Convergence check: L2 norm of change in eigenvector
        double delta = 0.0;
        for (int i = 0; i < n; i++) {
            double diff = v_new[i] - eigenvector[i];
            delta += diff * diff;
        }
        delta = std::sqrt(delta);

        eigenvector = v_new;
        iters = k + 1;

        if (delta < tol) {
            break;
        }
    }

    return eigenvalue;
}

// ─── Public API ──────────────────────────────────────────────────────────────

double EigenPower::inverseMethod(const Matrix& A,
                                 std::vector<double>& eigenvector,
                                 double tol,
                                 int maxIter) {
    int iters = 0;
    return inverseMethodImpl(A, eigenvector, tol, maxIter, iters);
}

double EigenPower::inverseMethod(const Matrix& A,
                                 std::vector<double>& eigenvector,
                                 double tol,
                                 int maxIter,
                                 int& iters) {
    return inverseMethodImpl(A, eigenvector, tol, maxIter, iters);
}