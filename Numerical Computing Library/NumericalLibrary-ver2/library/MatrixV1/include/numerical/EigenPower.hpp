#ifndef EIGEN_POWER_HPP
#define EIGEN_POWER_HPP

#include "../Matrix.hpp"
#include <vector>

class EigenPower {
public:
    static double powerMethod(const Matrix& A,
                              std::vector<double>& eigenvector,
                              int iterations = 100);
};

#endif