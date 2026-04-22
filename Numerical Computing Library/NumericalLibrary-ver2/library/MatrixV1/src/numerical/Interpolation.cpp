#include "../../include/numerical/Interpolation.hpp"
#include <stdexcept>

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

// Newton's divided difference interpolation.
//
// Builds the divided difference table dd[] where dd[k] holds the k-th order
// divided difference coefficient (i.e. f[x_0, x_1, ..., x_k]).
// The table is computed in-place using a single 1D vector.
//
// The Newton polynomial is then:
//   p(t) = dd[0]
//         + dd[1] * (t - x[0])
//         + dd[2] * (t - x[0])(t - x[1])
//         + ...
//         + dd[n-1] * (t - x[0])...(t - x[n-2])
//
// Evaluation uses Horner's method working right-to-left:
//   p(t) = dd[n-1]
//   p(t) = p(t) * (t - x[n-2]) + dd[n-2]
//   ...
//   p(t) = p(t) * (t - x[0])   + dd[0]
double Interpolation::newton(const std::vector<double>& x,
                             const std::vector<double>& y,
                             double value) {
    int n = static_cast<int>(x.size());

    if (n == 0) {
        throw std::invalid_argument("Newton interpolation: data points cannot be empty");
    }
    if (static_cast<int>(y.size()) != n) {
        throw std::invalid_argument("Newton interpolation: x and y must have the same size");
    }

    // Copy y into the divided difference working array
    std::vector<double> dd(y.begin(), y.end());

    // Build divided difference table in-place (upper triangular, stored column by column)
    // After k-th pass, dd[i] holds f[x_i, x_{i+1}, ..., x_{i+k}]
    for (int k = 1; k < n; k++) {
        for (int i = n - 1; i >= k; i--) {
            dd[i] = (dd[i] - dd[i - 1]) / (x[i] - x[i - k]);
        }
    }
    // Now dd[k] = f[x_0, x_1, ..., x_k] — the Newton coefficients

    // Evaluate using Horner's method (right-to-left)
    double result = dd[n - 1];
    for (int i = n - 2; i >= 0; i--) {
        result = result * (value - x[i]) + dd[i];
    }

    return result;
}