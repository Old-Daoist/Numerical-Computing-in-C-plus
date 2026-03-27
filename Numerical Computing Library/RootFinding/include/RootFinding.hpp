#pragma once
#include <functional>
#include <stdexcept>
#include <cmath>
#include <string>

namespace Numerical {

/**
 * @brief Result structure returned by all root-finding methods.
 */
struct RootResult {
    double root;          ///< Approximated root
    double fx;            ///< f(root) — residual
    int    iterations;    ///< Number of iterations taken
    bool   converged;     ///< Whether the method converged
    std::string method;   ///< Name of the method used
};

/**
 * @class RootFinder
 * @brief A collection of numerical root-finding algorithms.
 *
 * All methods solve f(x) = 0 for a given continuous function f.
 *
 * Usage:
 * @code
 *   auto f  = [](double x){ return x*x - 2; };
 *   auto df = [](double x){ return 2*x; };
 *   RootFinder rf;
 *   RootResult r = rf.newton(f, df, 1.0);
 *   std::cout << r.root; // ≈ 1.41421
 * @endcode
 */
class RootFinder {
public:
    // ── Constructor / configuration ──────────────────────────────────────────

    /**
     * @param tol       Convergence tolerance (default 1e-10)
     * @param maxIter   Maximum iterations (default 1000)
     */
    explicit RootFinder(double tol = 1e-10, int maxIter = 1000);

    void setTolerance(double tol);
    void setMaxIterations(int maxIter);
    double getTolerance()    const;
    int    getMaxIterations() const;

    // ── Bracketing methods ────────────────────────────────────────────────────

    /**
     * @brief Bisection Method.
     * Repeatedly bisects [a, b] where f(a)·f(b) < 0.
     * Guaranteed to converge; linear rate O(1/2^n).
     *
     * @param f   Function f(x)
     * @param a   Left  bracket
     * @param b   Right bracket
     */
    RootResult bisection(std::function<double(double)> f,
                         double a, double b) const;

    /**
     * @brief Regula Falsi (False Position) Method.
     * Uses linear interpolation of f(a) and f(b) to pick the next point.
     * Faster than Bisection but may stagnate on one side.
     */
    RootResult regulaFalsi(std::function<double(double)> f,
                           double a, double b) const;

    /**
     * @brief Illinois Method (improved Regula Falsi).
     * Scales f-values to prevent stagnation; super-linear convergence.
     */
    RootResult illinois(std::function<double(double)> f,
                        double a, double b) const;

    /**
     * @brief Brent's Method.
     * Combines Bisection, Secant, and Inverse-Quadratic Interpolation.
     * Guaranteed convergence; nearly optimal convergence rate.
     */
    RootResult brent(std::function<double(double)> f,
                     double a, double b) const;

    // ── Open methods ──────────────────────────────────────────────────────────

    /**
     * @brief Newton-Raphson Method.
     * Uses f and its derivative f' to iterate x_{n+1} = x_n - f(x_n)/f'(x_n).
     * Quadratic convergence near the root; may diverge without good initial guess.
     *
     * @param f   Function f(x)
     * @param df  Derivative f'(x)
     * @param x0  Initial guess
     */
    RootResult newton(std::function<double(double)> f,
                      std::function<double(double)> df,
                      double x0) const;

    /**
     * @brief Halley's Method.
     * Uses f, f', and f''; cubic convergence.
     * x_{n+1} = x_n - 2f·f' / (2f'^2 - f·f'')
     */
    RootResult halley(std::function<double(double)> f,
                      std::function<double(double)> df,
                      std::function<double(double)> d2f,
                      double x0) const;

    /**
     * @brief Secant Method.
     * Finite-difference approximation of Newton; does not require f'.
     * Super-linear convergence (~1.618).
     *
     * @param x0  First  initial point
     * @param x1  Second initial point
     */
    RootResult secant(std::function<double(double)> f,
                      double x0, double x1) const;

    /**
     * @brief Fixed-Point Iteration.
     * Rewrites f(x)=0 as x = g(x) and iterates.
     * Converges when |g'(x*)| < 1 near the root.
     *
     * @param g   Iteration function
     * @param x0  Initial guess
     */
    RootResult fixedPoint(std::function<double(double)> g,
                          double x0) const;

    /**
     * @brief Steffensen's Method.
     * Aitken Δ²-accelerated Fixed-Point; quadratic convergence without f'.
     */
    RootResult steffensen(std::function<double(double)> g,
                          double x0) const;

    /**
     * @brief Ridder's Method.
     * Bracketing + exponential transformation; super-linear convergence.
     */
    RootResult ridder(std::function<double(double)> f,
                      double a, double b) const;

    // ── Utility ───────────────────────────────────────────────────────────────

    /**
     * @brief Print a formatted summary of a RootResult.
     */
    static void printResult(const RootResult& r);

private:
    double tol_;
    int    maxIter_;
};

} // namespace Numerical