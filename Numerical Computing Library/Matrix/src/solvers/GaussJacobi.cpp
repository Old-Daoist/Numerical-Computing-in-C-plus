#include "../../include/solvers/SolveLinearEquation.hpp"

std::vector<double> GaussJacobi::solve(const Matrix& A, const std::vector<double>& b, int iterations) {
    int n = A.getRows();
    std::vector<double> x(n, 0), newX(n, 0);

    for(int it = 0; it < iterations; it++) {
        for(int i = 0; i < n; i++) {
            double sum = b[i];

            for(int j = 0; j < n; j++) {
                if(j != i)
                    sum -= A.get(i,j) * x[j];
            }

            newX[i] = sum / A.get(i,i);
        }
        x = newX;
    }

    return x;
}