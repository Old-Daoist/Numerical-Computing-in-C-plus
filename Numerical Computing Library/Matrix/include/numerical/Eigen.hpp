#ifndef EIGEN_HPP
#define EIGEN_HPP

#include "../Matrix.hpp"

class Eigen {
public:
    static void gershgorin(const Matrix& A);
};

#endif