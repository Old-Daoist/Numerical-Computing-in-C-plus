#ifndef GAUSS_JACOBI_HPP
#define GAUSS_JACOBI_HPP

#include "SolveLinearEquation.hpp"

class GaussJacobi : public SolveLinearEquation {
public:
    GaussJacobi(int n);
    std::vector<double> solve();
};

#endif