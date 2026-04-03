#ifndef GAUSS_ELIMINATION_HPP
#define GAUSS_ELIMINATION_HPP

#include "SolveLinearEquations.hpp"

class GaussElimination : public SolveLinearEquation {
public:
    GaussElimination(int n);
    std::vector<double> solve();
};

#endif