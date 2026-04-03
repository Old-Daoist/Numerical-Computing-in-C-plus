#ifndef LU_DECOMPOSITION_HPP
#define LU_DECOMPOSITION_HPP

#include "Matrix.hpp"

class LUDecomposition : public Matrix {
public:
    LUDecomposition(int n);

    void decompose(std::vector<std::vector<double>>& L,
                   std::vector<std::vector<double>>& U);
};

#endif