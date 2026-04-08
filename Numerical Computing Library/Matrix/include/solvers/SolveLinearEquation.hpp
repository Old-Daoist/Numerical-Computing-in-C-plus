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
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b, int iterations = 25);
};

class GaussSeidel {
public:
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b, int iterations = 25);
};

#endif