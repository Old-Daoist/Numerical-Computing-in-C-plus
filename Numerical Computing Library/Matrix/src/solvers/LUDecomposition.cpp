#include "../../include/solvers/LUDecomposition.hpp"
#include <stdexcept>

// LU Decomposition (Doolittle)
void LU::decompose(const Matrix& A, Matrix& L, Matrix& U) {
    int n = A.getRows();

    for(int i = 0; i < n; i++) {

        // Upper
        for(int k = i; k < n; k++) {
            double sum = 0;
            for(int j = 0; j < i; j++)
                sum += L.get(i,j) * U.get(j,k);

            U.set(i,k, A.get(i,k) - sum);
        }

        // Lower
        for(int k = i; k < n; k++) {
            if(i == k)
                L.set(i,i, 1);
            else {
                double sum = 0;
                for(int j = 0; j < i; j++)
                    sum += L.get(k,j) * U.get(j,i);

                L.set(k,i, (A.get(k,i) - sum) / U.get(i,i));
            }
        }
    }
}

// Solve using LU
std::vector<double> LU::solve(const Matrix& A, const std::vector<double>& b) {
    int n = A.getRows();

    Matrix L(n,n), U(n,n);
    decompose(A, L, U);

    // Forward substitution Ly = b
    std::vector<double> y(n);
    for(int i = 0; i < n; i++) {
        y[i] = b[i];
        for(int j = 0; j < i; j++)
            y[i] -= L.get(i,j) * y[j];
    }

    // Backward substitution Ux = y
    std::vector<double> x(n);
    for(int i = n-1; i >= 0; i--) {
        x[i] = y[i];
        for(int j = i+1; j < n; j++)
            x[i] -= U.get(i,j) * x[j];

        x[i] /= U.get(i,i);
    }

    return x;
}