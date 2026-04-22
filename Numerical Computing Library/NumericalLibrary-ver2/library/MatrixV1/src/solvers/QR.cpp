#include "../../include/solvers/QR.hpp"
#include <cmath>
#include <stdexcept>
#include <algorithm>

// Helper: compute L2 norm of a vector
static double vectorL2Norm(const std::vector<double>& v) {
    double sum = 0.0;
    for (double val : v) {
        sum += val * val;
    }
    return std::sqrt(sum);
}

// Helper: compute dot product
static double dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

QR::Decomposition QR::decompose(const Matrix& A) {
    int m = A.getRows();
    int n = A.getCols();
    
    if (m < n) {
        throw std::runtime_error("QR: Matrix must have m >= n (more rows than columns)");
    }
    
    // Start with A as the working matrix, will build Q and R
    Matrix Q = A;  // We'll overwrite this with Q columns
    Matrix R(n, n);
    
    // Householder QR: process each column
    for (int j = 0; j < n; j++) {
        // Extract column j from row j onwards
        std::vector<double> x(m - j);
        for (int i = j; i < m; i++) {
            x[i - j] = Q.get(i, j);
        }
        
        // Compute norm
        double sigma = vectorL2Norm(x);
        
        if (sigma < 1e-12) {
            throw std::runtime_error("QR: Matrix appears to be rank-deficient at column " + std::to_string(j));
        }
        
        // Householder vector: v = x + sign(x[0]) * sigma * e_1
        std::vector<double> v = x;
        v[0] += (x[0] >= 0 ? 1 : -1) * sigma;
        
        // Normalize v
        double v_norm = vectorL2Norm(v);
        if (v_norm > 1e-12) {
            for (double& val : v) {
                val /= v_norm;
            }
        }
        
        // Apply Householder transformation to columns j..n-1 of Q
        for (int k = j; k < n; k++) {
            // Extract column k from row j onwards
            std::vector<double> col(m - j);
            for (int i = j; i < m; i++) {
                col[i - j] = Q.get(i, k);
            }
            
            // Compute projection: 2 * (v^T * col) * v
            double vT_col = dotProduct(v, col);
            
            // Update column: col = col - 2 * (v^T * col) * v
            for (int i = 0; i < (int)col.size(); i++) {
                col[i] -= 2.0 * vT_col * v[i];
            }
            
            // Write back to Q
            for (int i = j; i < m; i++) {
                Q.set(i, k, col[i - j]);
            }
        }
        
        // Extract R[j][j] = ||x||
        R.set(j, j, sigma);
        
        // Extract R[j][k] for k > j from transformed Q
        for (int k = j + 1; k < n; k++) {
            R.set(j, k, Q.get(j, k));
        }
    }
    
    // Now Q contains the QR factors but in a form we need to extract
    // For simplicity, use a different approach: classical Gram-Schmidt
    // (less stable but simpler to implement correctly)
    
    // Reset and use modified Gram-Schmidt
    Matrix A_copy = A;
    Matrix Q_gs(m, n);
    Matrix R_gs(n, n);
    
    for (int j = 0; j < n; j++) {
        // Extract column j
        std::vector<double> col(m);
        for (int i = 0; i < m; i++) {
            col[i] = A_copy.get(i, j);
        }
        
        // Gram-Schmidt: orthogonalize against previous columns
        for (int i = 0; i < j; i++) {
            // Get Q column i
            std::vector<double> q_i(m);
            for (int k = 0; k < m; k++) {
                q_i[k] = Q_gs.get(k, i);
            }
            
            // Compute R[i][j] = Q[:,i]^T * A[:,j]
            double r_ij = dotProduct(q_i, col);
            R_gs.set(i, j, r_ij);
            
            // Subtract: col = col - r_ij * Q[:,i]
            for (int k = 0; k < m; k++) {
                col[k] -= r_ij * q_i[k];
            }
        }
        
        // Normalize current column
        double norm = vectorL2Norm(col);
        if (norm < 1e-12) {
            throw std::runtime_error("QR: Matrix appears to be rank-deficient at column " + std::to_string(j));
        }
        
        R_gs.set(j, j, norm);
        
        // Store normalized column in Q
        for (int i = 0; i < m; i++) {
            Q_gs.set(i, j, col[i] / norm);
        }
    }
    
    return {Q_gs, R_gs};
}

std::vector<double> QR::solve(const Matrix& A, const std::vector<double>& b) {
    int m = A.getRows();
    int n = A.getCols();
    
    if ((int)b.size() != m) {
        throw std::runtime_error("QR solve: b must have m elements");
    }
    
    // Compute QR decomposition
    Decomposition qr = decompose(A);
    
    // Compute c = Q^T * b
    std::vector<double> c(n);
    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int j = 0; j < m; j++) {
            sum += qr.Q.get(j, i) * b[j];
        }
        c[i] = sum;
    }
    
    // Solve R*x = c by back substitution
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        double sum = c[i];
        for (int j = i + 1; j < n; j++) {
            sum -= qr.R.get(i, j) * x[j];
        }
        
        double r_ii = qr.R.get(i, i);
        if (std::abs(r_ii) < 1e-12) {
            throw std::runtime_error("QR solve: Singular matrix encountered");
        }
        
        x[i] = sum / r_ii;
    }
    
    return x;
}