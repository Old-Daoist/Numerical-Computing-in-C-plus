#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include <vector>

class Interpolation {
public:
    // Lagrange interpolation — original method
    static double lagrange(const std::vector<double>& x,
                           const std::vector<double>& y,
                           double value);

    // Newton's divided difference interpolation
    // Mathematically equivalent to Lagrange on same data (result matches within 1e-9).
    // More efficient for multiple query points since the divided difference table
    // only needs to be built once (O(n²) build, O(n) per query).
    // Throws std::invalid_argument if x and y have different sizes or are empty.
    static double newton(const std::vector<double>& x,
                         const std::vector<double>& y,
                         double value);
};

#endif