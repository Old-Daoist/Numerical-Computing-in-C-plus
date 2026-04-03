#ifndef EIGEN_VALUE_HPP
#define EIGEN_VALUE_HPP

#include "Matrix.hpp"

class EigenValue : public Matrix {
public:
    EigenValue(int n);
    double powerMethod();
};

#endif