#include "../include/GaussSeidel.hpp"

GaussSeidel::GaussSeidel(int n) : SolveLinearEquation(n) {}

std::vector<double> GaussSeidel::solve() {
    std::vector<double> X(n, 0);

    for(int iter = 0; iter < 1000; iter++) {
        for(int i = 0; i < n; i++) {
            double sum = B[i];
            double diag = 1;

            for(int k = rowPtr[i]; k < rowPtr[i + 1]; k++) {
                int j = colIndex[k];

                if(i == j)
                    diag = values[k];
                else
                    sum -= values[k] * X[j];
            }

            X[i] = sum / diag;
        }
    }

    return X;
}