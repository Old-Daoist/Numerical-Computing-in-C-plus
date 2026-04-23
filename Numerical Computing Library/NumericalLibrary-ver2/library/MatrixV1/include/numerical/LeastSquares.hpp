#ifndef LEAST_SQUARES_HPP
#define LEAST_SQUARES_HPP

#include <vector>
#include <utility>   // std::pair
#include <tuple>     // std::tuple
#include <stdexcept>

/// Least squares curve fitting via normal equations.
///
/// Both methods build the normal-equation system from single-pass sums of
/// powers of x and mixed xy products, then solve the resulting small linear
/// system using GaussElimination::solve().  No new solver infrastructure is
/// required.
class LeastSquares {
public:
    /// Fit  y = a + b*x  to n data points by minimising Σ(yᵢ - a - b·xᵢ)².
    ///
    /// Returns {a, b} — intercept and slope of the best-fit line.
    ///
    /// Throws std::invalid_argument if:
    ///   - x or y is empty, or x.size() != y.size()
    ///   - fewer than 2 data points (underdetermined)
    ///   - all x values are identical (singular normal-equation matrix)
    static std::pair<double, double>
    fitLine(const std::vector<double>& x,
            const std::vector<double>& y);

    /// Fit  y = a + b*x + c*x²  to n data points by minimising
    /// Σ(yᵢ - a - b·xᵢ - c·xᵢ²)².
    ///
    /// Returns {a, b, c} — intercept, linear, and quadratic coefficients.
    ///
    /// Throws std::invalid_argument if:
    ///   - x or y is empty, or x.size() != y.size()
    ///   - fewer than 3 data points (underdetermined)
    ///   - normal-equation matrix is singular (degenerate x distribution)
    static std::tuple<double, double, double>
    fitParabola(const std::vector<double>& x,
                const std::vector<double>& y);
};

#endif
