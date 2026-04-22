#include "../../include/solvers/QR.hpp"
#include <cmath>
#include <stdexcept>

// Helper: compute L2 norm of a vector
static double vectorL2Norm(const std::vector<double>& v) {
    double sum = 0.0;
    for (double val : v) {
        sum += val * val;
    }
    return std::sqrt(sum);
}

// Helper: compute dot product of two vectors
static double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// QR decomposition using Modified Gram-Schmidt (MGS)
// MGS is more numerically stable than Classical Gram-Schmidt.
// For each column j, we orthogonalize it against all previous Q columns,
// and — crucially — we update the *working* column after each projection
// (modified GS) rather than computing all projections from the original column
// (classical GS). This reduces loss of orthogonality.
QR::Decomposition QR::decompose(const Matrix& A) {
    int m = A.getRows();
    int n = A.getCols();

    if (m < n) {
        throw std::runtime_error("QR: Matrix must have m >= n (more rows than columns)");
    }

    // Working copy — columns will be modified in-place during orthogonalization
    Matrix W = A;
    Matrix Q(m, n);
    Matrix R(n, n);

    for (int j = 0; j < n; j++) {
        // Extract working column j
        std::vector<double> col(m);
        for (int i = 0; i < m; i++) {
            col[i] = W.get(i, j);
        }

        // Modified Gram-Schmidt: orthogonalize against all previous Q columns
        for (int k = 0; k < j; k++) {
            // Build Q column k
            std::vector<double> q_k(m);
            for (int i = 0; i < m; i++) {
                q_k[i] = Q.get(i, k);
            }

            // R[k][j] = q_k^T * col  (projection coefficient)
            double r_kj = dotProduct(q_k, col);
            R.set(k, j, r_kj);

            // Subtract projection: col = col - r_kj * q_k
            for (int i = 0; i < m; i++) {
                col[i] -= r_kj * q_k[i];
            }
        }

        // Compute norm of residual column
        double norm = vectorL2Norm(col);
        if (norm < 1e-12) {
            throw std::runtime_error(
                "QR: Matrix appears to be rank-deficient at column " + std::to_string(j));
        }

        // R[j][j] = norm
        R.set(j, j, norm);

        // Normalize and store as Q column j
        for (int i = 0; i < m; i++) {
            Q.set(i, j, col[i] / norm);
        }
    }

    return {Q, R};
}

// Solve Ax = b via QR decomposition.
// For square systems (m == n): exact solution.
// For overdetermined systems (m > n): least-squares solution  min ||Ax - b||_2.
// Both cases reduce to solving  R x = Q^T b  by back-substitution.
std::vector<double> QR::solve(const Matrix& A, const std::vector<double>& b) {
    int m = A.getRows();
    int n = A.getCols();

    if (static_cast<int>(b.size()) != m) {
        throw std::runtime_error("QR solve: b must have m elements");
    }

    // Step 1: Compute QR
    Decomposition qr = decompose(A);

    // Step 2: Compute c = Q^T * b  (n-vector)
    std::vector<double> c(n);
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int j = 0; j < m; j++) {
            sum += qr.Q.get(j, i) * b[j];
        }
        c[i] = sum;
    }

    // Step 3: Solve R*x = c by back substitution
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        double sum = c[i];
        for (int j = i + 1; j < n; j++) {
            sum -= qr.R.get(i, j) * x[j];
        }

        double r_ii = qr.R.get(i, i);
        if (std::abs(r_ii) < 1e-12) {
            throw std::runtime_error("QR solve: Singular R diagonal encountered");
        }

        x[i] = sum / r_ii;
    }

    return x;
}