#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

#include "Matrix.hpp"

class Multiply {
public:
    static Matrix compute(const Matrix& A, const Matrix& B);
};

#endif