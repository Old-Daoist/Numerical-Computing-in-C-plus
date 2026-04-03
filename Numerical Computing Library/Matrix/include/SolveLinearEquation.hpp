#ifndef SOLVE_LINEAR_EQUATION_HPP
#define SOLVE_LINEAR_EQUATION_HPP

#include "SparseMatrix.hpp"
#include <vector>

class SolveLinearEquation : public SparseMatrix {
public:
    SolveLinearEquation(int n);

    virtual std::vector<double> solve() = 0;

    bool isDiagonallyDominant();
};

#endif