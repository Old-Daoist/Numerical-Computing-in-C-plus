#ifndef CHOLESKY_HPP
#define CHOLESKY_HPP

#include "Matrix.hpp"
#include <vector>

// Cholesky decomposition for symmetric positive-definite (SPD) matrices
// For SPD matrix A, computes L (lower triangular) such that A = L * L^T
// More efficient than LU for SPD systems: O(n³/3) vs O(n³/3) for LU but no pivoting needed
class Cholesky {
public:
    // Decompose A into L where A = L * L^T
    // Throws std::runtime_error if A is not symmetric positive-definite
    static Matrix decompose(const Matrix& A);
    
    // Solve Ax = b using Cholesky decomposition
    // First computes L from A, then solves L*y=b and L^T*x=y
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b);
};

#endif