#ifndef EIGEN_POWER_HPP
#define EIGEN_POWER_HPP

#include "../Matrix.hpp"
#include <vector>

class EigenPower {
public:
    // Power method — dominant (largest magnitude) eigenvalue.
    // Fixed iteration count; original interface preserved for backward compatibility.
    static double powerMethod(const Matrix& A,
                              std::vector<double>& eigenvector,
                              int iterations = 100);

    // Inverse power method — smallest magnitude eigenvalue.
    // Each iteration solves A*w = v (via LU) and normalises.
    // Convergence: stops when ||v_new - v_old||_2 < tol or maxIter reached.
    // Throws std::runtime_error if A is not square or is singular.
    static double inverseMethod(const Matrix& A,
                                std::vector<double>& eigenvector,
                                double tol = 1e-9,
                                int maxIter = 1000);

    // Overload that also reports the number of iterations performed.
    static double inverseMethod(const Matrix& A,
                                std::vector<double>& eigenvector,
                                double tol, int maxIter, int& iters);
};

#endif