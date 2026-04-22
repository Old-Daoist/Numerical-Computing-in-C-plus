#include "../../include/numerical/Interpolation.hpp"

double Interpolation::lagrange(const std::vector<double>& x,
                               const std::vector<double>& y,
                               double value) {
    int n = x.size();
    double result = 0;

    for(int i = 0; i < n; i++) {
        double term = y[i];

        for(int j = 0; j < n; j++) {
            if(j != i) {
                term *= (value - x[j]) / (x[i] - x[j]);
            }
        }

        result += term;
    }

    return result;
}