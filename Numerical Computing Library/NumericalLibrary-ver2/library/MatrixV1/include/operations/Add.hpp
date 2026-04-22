#ifndef ADD_HPP
#define ADD_HPP

#include "Matrix.hpp"

class Add {
public:
    static Matrix compute(const Matrix& A, const Matrix& B);
};

#endif