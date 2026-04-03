#include "../include/GaussElimination.hpp"

GaussElimination::GaussElimination(int n) : SolveLinearEquation(n) {}

std::vector<double> GaussElimination::solve() {
    for(int k=0;k<n-1;k++)
        for(int i=k+1;i<n;i++) {
            double f = A[i][k]/A[k][k];
            for(int j=k;j<n;j++)
                A[i][j] -= f*A[k][j];
            B[i] -= f*B[k];
        }

    std::vector<double> X(n);

    for(int i=n-1;i>=0;i--) {
        X[i] = B[i];
        for(int j=i+1;j<n;j++)
            X[i] -= A[i][j]*X[j];
        X[i] /= A[i][i];
    }

    return X;
}