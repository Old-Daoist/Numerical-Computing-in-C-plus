#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include <vector>

class Interpolation {
public:
    static double lagrange(const std::vector<double>& x,
                           const std::vector<double>& y,
                           double value);
};

#endif