#include "../include/SolveLinearEquation.hpp"
#include <cmath>

SolveLinearEquation::SolveLinearEquation(int n) : SparseMatrix(n) {}

bool SolveLinearEquation::isDiagonallyDominant() {
    for(int i = 0; i < n; i++) {
        double diag = 0, sum = 0;

        for(int k = rowPtr[i]; k < rowPtr[i + 1]; k++) {
            if(colIndex[k] == i)
                diag = std::abs(values[k]);
            else
                sum += std::abs(values[k]);
        }

        if(diag < sum)
            return false;
    }
    return true;
}