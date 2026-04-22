#ifndef SOLVE_LINEAR_EQUATION_HPP
#define SOLVE_LINEAR_EQUATION_HPP

#include "Matrix.hpp"
#include <vector>

class GaussElimination {
public:
    static std::vector<double> solve(Matrix A, std::vector<double> b);
};

class GaussJacobi {
public:
    // Original: fixed iteration count
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b, int iterations = 25);
    
    // New: convergence-based with tolerance
    // Returns solution and updates iters with the number of iterations performed
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b, 
                                     double tol, int maxIter, int& iters);
};

class GaussSeidel {
public:
    // Original: fixed iteration count
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b, int iterations = 25);
    
    // New: convergence-based with tolerance
    // Returns solution and updates iters with the number of iterations performed
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b, 
                                     double tol, int maxIter, int& iters);
};

#endif