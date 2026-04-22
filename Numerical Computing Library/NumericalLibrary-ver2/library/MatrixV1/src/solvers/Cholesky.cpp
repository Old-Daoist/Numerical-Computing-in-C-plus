#include "../../include/solvers/Cholesky.hpp"
#include <cmath>
#include <stdexcept>

Matrix Cholesky::decompose(const Matrix& A) {
    if (!A.isSquare()) {
        throw std::runtime_error("Cholesky: Matrix must be square");
    }
    
    int n = A.getRows();
    Matrix L(n, n);  // Initialize with zeros
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0.0;
            
            if (i == j) {
                // Diagonal element: L[i][i] = sqrt(A[i][i] - sum(L[i][k]^2))
                sum = A.get(i, i);
                for (int k = 0; k < j; k++) {
                    sum -= L.get(i, k) * L.get(i, k);
                }
                
                // Check for positive-definiteness
                if (sum <= 1e-12) {
                    throw std::runtime_error("Cholesky: Matrix is not positive-definite (negative diagonal element)");
                }
                
                L.set(i, i, std::sqrt(sum));
            } else {
                // Below-diagonal: L[i][j] = (A[i][j] - sum(L[i][k]*L[j][k])) / L[j][j]
                sum = A.get(i, j);
                for (int k = 0; k < j; k++) {
                    sum -= L.get(i, k) * L.get(j, k);
                }
                
                double L_jj = L.get(j, j);
                if (std::abs(L_jj) < 1e-12) {
                    throw std::runtime_error("Cholesky: Zero pivot encountered");
                }
                
                L.set(i, j, sum / L_jj);
            }
        }
    }
    
    return L;
}

std::vector<double> Cholesky::solve(const Matrix& A, const std::vector<double>& b) {
    // Step 1: Compute L from A = L*L^T
    Matrix L = decompose(A);
    
    int n = A.getRows();
    std::vector<double> y(n), x(n);
    
    // Step 2: Solve L*y = b (forward substitution)
    for (int i = 0; i < n; i++) {
        double sum = b[i];
        for (int j = 0; j < i; j++) {
            sum -= L.get(i, j) * y[j];
        }
        y[i] = sum / L.get(i, i);
    }
    
    // Step 3: Solve L^T*x = y (back substitution)
    for (int i = n - 1; i >= 0; i--) {
        double sum = y[i];
        for (int j = i + 1; j < n; j++) {
            sum -= L.get(j, i) * x[j];  // L^T[i][j] = L[j][i]
        }
        x[i] = sum / L.get(i, i);
    }
    
    return x;
}