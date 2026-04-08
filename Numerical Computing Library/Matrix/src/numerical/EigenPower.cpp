#include "../../include/numerical/EigenPower.hpp"
#include <cmath>
#include <stdexcept>

double EigenPower::powerMethod(const Matrix& A,
                               std::vector<double>& eigenvector,
                               int iterations) {

    int n = A.getRows();

    if(A.getRows() != A.getCols())
        throw std::runtime_error("Matrix must be square");

    eigenvector.assign(n, 1.0);

    double eigenvalue = 0;

    for(int it = 0; it < iterations; it++) {
        std::vector<double> temp(n, 0);

        // Multiply A * x
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                temp[i] += A.get(i,j) * eigenvector[j];
            }
        }

        // Find max element (normalization)
        double maxVal = std::abs(temp[0]);
        for(int i = 1; i < n; i++) {
            if(std::abs(temp[i]) > maxVal)
                maxVal = std::abs(temp[i]);
        }

        if(maxVal < 1e-12)
            throw std::runtime_error("Power method failed (zero vector)");

        // Normalize
        for(int i = 0; i < n; i++) {
            eigenvector[i] = temp[i] / maxVal;
        }

        eigenvalue = maxVal;
    }

    return eigenvalue;
}