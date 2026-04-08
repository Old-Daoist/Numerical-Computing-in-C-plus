#include "../include/SolveLinearEquation.hpp"
#include <stdexcept>

std::vector<double> GaussElimination::solve(Matrix A, std::vector<double> b) {
    int n = A.getRows();

    for(int i = 0; i < n; i++) {
        for(int j = i + 1; j < n; j++) {
            double factor = A.get(j,i) / A.get(i,i);

            for(int k = 0; k < n; k++) {
                A.set(j,k, A.get(j,k) - factor * A.get(i,k));
            }

            b[j] -= factor * b[i];
        }
    }

    std::vector<double> x(n);

    for(int i = n - 1; i >= 0; i--) {
        x[i] = b[i];
        for(int j = i + 1; j < n; j++) {
            x[i] -= A.get(i,j) * x[j];
        }
        x[i] /= A.get(i,i);
    }

    return x;
}