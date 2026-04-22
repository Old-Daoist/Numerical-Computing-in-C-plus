#include "../../include/solvers/SolveLinearEquation.hpp"
#include <cmath>

// Helper: compute L2-norm of difference between two vectors
static double vectorL2Norm(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

std::vector<double> GaussSeidel::solve(const Matrix& A, const std::vector<double>& b, int iterations) {
    int n = A.getRows();
    std::vector<double> x(n, 0);
    for(int i = 0; i < n; i++) {
    if(std::abs(A.get(i,i)) < 1e-12)
        throw std::runtime_error("Zero diagonal — method invalid");
}

    for(int it = 0; it < iterations; it++) {
        for(int i = 0; i < n; i++) {
            double sum = b[i];

            for(int j = 0; j < n; j++) {
                if(j != i)
                    sum -= A.get(i,j) * x[j];
            }

            x[i] = sum / A.get(i,i);
        }
    }

    return x;
}

// New overload: convergence-based with tolerance
std::vector<double> GaussSeidel::solve(const Matrix& A, const std::vector<double>& b, 
                                       double tol, int maxIter, int& iters) {
    int n = A.getRows();
    std::vector<double> x(n, 0), xPrev(n, 0);
    
    // Check for zero diagonal
    for(int i = 0; i < n; i++) {
        if(std::abs(A.get(i,i)) < 1e-12)
            throw std::runtime_error("Zero diagonal — method invalid");
    }

    iters = 0;
    for(int it = 0; it < maxIter; it++) {
        xPrev = x;  // Store previous iteration
        
        for(int i = 0; i < n; i++) {
            double sum = b[i];

            for(int j = 0; j < n; j++) {
                if(j != i)
                    sum -= A.get(i,j) * x[j];
            }

            x[i] = sum / A.get(i,i);
        }

        // Check convergence: ||x_new - x_prev||_2 < tol
        double delta = vectorL2Norm(x, xPrev);
        iters++;
        
        if (delta < tol) {
            return x;  // Converged
        }
    }

    // Reached max iterations without converging
    return x;
}