#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

namespace Numerical {

/**
 * @class Complex
 * @brief Full-featured complex number class with arithmetic, transcendental,
 *        and utility operations.
 *
 * Stores a complex number z = re + i·im in Cartesian form.
 *
 * Usage:
 * @code
 *   Complex a(3, 4);            // 3 + 4i
 *   Complex b = Complex::polar(5, M_PI/4); // from polar form
 *   std::cout << (a * b).modulus(); // |a||b|
 * @endcode
 */
class Complex {
public:
    // ── Constructors ──────────────────────────────────────────────────────────

    Complex();                                  ///< Zero:  0 + 0i
    Complex(double real, double imag = 0.0);    ///< Cartesian: re + i·im

    /** Construct from polar form: r·e^{iθ} = r·cos(θ) + i·r·sin(θ) */
    static Complex polar(double r, double theta);

    /** Construct from exponential form: e^{a + ib} = e^a·(cos b + i·sin b) */
    static Complex fromExp(double a, double b);

    // ── Accessors ─────────────────────────────────────────────────────────────

    double real() const;     ///< Real part (Re z)
    double imag() const;     ///< Imaginary part (Im z)
    void   setReal(double r);
    void   setImag(double i);

    // ── Arithmetic operators ──────────────────────────────────────────────────

    Complex operator+(const Complex& rhs) const;
    Complex operator-(const Complex& rhs) const;
    Complex operator*(const Complex& rhs) const;
    Complex operator/(const Complex& rhs) const;

    Complex& operator+=(const Complex& rhs);
    Complex& operator-=(const Complex& rhs);
    Complex& operator*=(const Complex& rhs);
    Complex& operator/=(const Complex& rhs);

    Complex operator-() const;   ///< Negation
    bool    operator==(const Complex& rhs) const;
    bool    operator!=(const Complex& rhs) const;

    // Scalar overloads
    Complex operator+(double s) const;
    Complex operator-(double s) const;
    Complex operator*(double s) const;
    Complex operator/(double s) const;

    // ── Properties ────────────────────────────────────────────────────────────

    /** |z| = sqrt(re² + im²) */
    double modulus()   const;

    /** |z|² = re² + im² (avoids sqrt) */
    double modulusSq() const;

    /** arg(z) ∈ (-π, π]; principal argument */
    double argument()  const;

    /** z̄ = re − i·im */
    Complex conjugate() const;

    /** 1/z */
    Complex inverse()  const;

    /** Normalize to unit modulus: z / |z| */
    Complex normalize() const;

    bool isReal()      const;   ///< Im z ≈ 0
    bool isPureImag()  const;   ///< Re z ≈ 0
    bool isZero()      const;   ///< z ≈ 0

    // ── Algebraic functions ───────────────────────────────────────────────────

    /** z^n for integer exponent */
    Complex pow(int n) const;

    /** z^w = exp(w · ln z) for complex exponent */
    Complex pow(const Complex& w) const;

    /** Principal square root */
    Complex sqrt() const;

    /** Natural logarithm: ln z = ln|z| + i·arg(z) */
    Complex log() const;

    /** log base b: log_b(z) = ln z / ln b */
    Complex log(const Complex& base) const;

    /** e^z = e^re · (cos(im) + i·sin(im)) */
    Complex exp() const;

    // ── Trigonometric functions ────────────────────────────────────────────────

    Complex sin()  const;   ///< sin z  = (e^{iz} − e^{−iz}) / 2i
    Complex cos()  const;   ///< cos z  = (e^{iz} + e^{−iz}) / 2
    Complex tan()  const;   ///< tan z  = sin z / cos z
    Complex cot()  const;
    Complex sec()  const;
    Complex csc()  const;

    // ── Hyperbolic functions ───────────────────────────────────────────────────

    Complex sinh() const;
    Complex cosh() const;
    Complex tanh() const;

    // ── Inverse trig ──────────────────────────────────────────────────────────

    Complex asin() const;
    Complex acos() const;
    Complex atan() const;

    // ── Roots ─────────────────────────────────────────────────────────────────

    /** All n-th roots of z: z^{1/n} for k = 0, 1, ..., n-1 */
    std::vector<Complex> nthRoots(int n) const;

    // ── Representation ────────────────────────────────────────────────────────

    /** Convert to polar form: {r, θ} */
    std::pair<double, double> toPolar() const;

    /** Formatted string: "a + bi", "a - bi", "bi", etc. */
    std::string toString(int precision = 6) const;

    /** Stream output */
    friend std::ostream& operator<<(std::ostream& os, const Complex& z);

    /** Stream input expects format "a+bi" or "a" or "bi" */
    friend std::istream& operator>>(std::istream& is, Complex& z);

    // Scalar-on-left friends
    friend Complex operator+(double s, const Complex& z);
    friend Complex operator-(double s, const Complex& z);
    friend Complex operator*(double s, const Complex& z);

    // ── Constants ─────────────────────────────────────────────────────────────

    static const Complex ZERO;   ///< 0 + 0i
    static const Complex ONE;    ///< 1 + 0i
    static const Complex I;      ///< 0 + 1i
    static const Complex NEG_ONE;///< -1 + 0i

private:
    double re_;
    double im_;

    static constexpr double EPS = 1e-12;
};

// ── Free function helpers ─────────────────────────────────────────────────────

Complex cexp  (const Complex& z);
Complex clog  (const Complex& z);
Complex csqrt (const Complex& z);
Complex cpow  (const Complex& base, const Complex& exp);
Complex csin  (const Complex& z);
Complex ccos  (const Complex& z);
Complex ctan  (const Complex& z);
Complex csinh (const Complex& z);
Complex ccosh (const Complex& z);
Complex ctanh (const Complex& z);

} // namespace Numerical