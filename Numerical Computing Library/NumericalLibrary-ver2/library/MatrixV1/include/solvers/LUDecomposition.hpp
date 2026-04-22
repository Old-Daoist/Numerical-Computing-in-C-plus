#ifndef LU_HPP
#define LU_HPP

#include "../Matrix.hpp"
#include <vector>

class LU {
public:
    static void decompose(const Matrix& A, Matrix& L, Matrix& U);
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b);
};

#endif