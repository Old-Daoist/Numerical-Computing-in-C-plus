#include "../../include/solvers/SolveLinearEquation.hpp"
#include <cmath>
#include <stdexcept>
#include <algorithm>

std::vector<double> GaussElimination::solve(Matrix A, std::vector<double> b) {
    int n = A.getRows();

    for(int i = 0; i < n; i++) {

        // PARTIAL PIVOTING
        int pivot = i;
        for(int p = i+1; p < n; p++) {
            if(std::abs(A.get(p,i)) > std::abs(A.get(pivot,i)))
                pivot = p;
        }

        if(std::abs(A.get(pivot,i)) < 1e-12)
            throw std::runtime_error("Singular matrix (zero pivot)");

        // Swap rows in A
        if(pivot != i) {
            for(int k = 0; k < n; k++) {
                double temp = A.get(i,k);
                A.set(i,k, A.get(pivot,k));
                A.set(pivot,k, temp);
            }
            std::swap(b[i], b[pivot]);
        }

        // Elimination
        for(int j = i + 1; j < n; j++) {
            double factor = A.get(j,i) / A.get(i,i);

            for(int k = i; k < n; k++) {
                A.set(j,k, A.get(j,k) - factor * A.get(i,k));
            }

            b[j] -= factor * b[i];
        }
    }

    // Back substitution
    std::vector<double> x(n);

    for(int i = n - 1; i >= 0; i--) {
        double diag = A.get(i,i);

        if(std::abs(diag) < 1e-12)
            throw std::runtime_error("Singular matrix in back substitution");

        x[i] = b[i];
        for(int j = i + 1; j < n; j++) {
            x[i] -= A.get(i,j) * x[j];
        }
        x[i] /= diag;
    }

    return x;
}