#ifndef GAUSS_SEIDEL_HPP
#define GAUSS_SEIDEL_HPP

#include "SolveLinearEquation.hpp"

class GaussSeidel : public SolveLinearEquation {
public:
    GaussSeidel(int n);
    std::vector<double> solve();
};

#endif