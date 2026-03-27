#include "../include/ComplexNumber.hpp"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

namespace Numerical {

// ── Static constants ──────────────────────────────────────────────────────────
const Complex Complex::ZERO    { 0.0,  0.0};
const Complex Complex::ONE     { 1.0,  0.0};
const Complex Complex::I       { 0.0,  1.0};
const Complex Complex::NEG_ONE {-1.0,  0.0};

// ── Constructors ──────────────────────────────────────────────────────────────

Complex::Complex() : re_(0.0), im_(0.0) {}
Complex::Complex(double real, double imag) : re_(real), im_(imag) {}

Complex Complex::polar(double r, double theta) {
    return Complex(r * std::cos(theta), r * std::sin(theta));
}

Complex Complex::fromExp(double a, double b) {
    double ea = std::exp(a);
    return Complex(ea * std::cos(b), ea * std::sin(b));
}

// ── Accessors ─────────────────────────────────────────────────────────────────

double Complex::real() const { return re_; }
double Complex::imag() const { return im_; }
void   Complex::setReal(double r) { re_ = r; }
void   Complex::setImag(double i) { im_ = i; }

// ── Arithmetic operators ──────────────────────────────────────────────────────

Complex Complex::operator+(const Complex& rhs) const { return {re_ + rhs.re_, im_ + rhs.im_}; }
Complex Complex::operator-(const Complex& rhs) const { return {re_ - rhs.re_, im_ - rhs.im_}; }

Complex Complex::operator*(const Complex& rhs) const {
    return {re_ * rhs.re_ - im_ * rhs.im_,
            re_ * rhs.im_ + im_ * rhs.re_};
}

Complex Complex::operator/(const Complex& rhs) const {
    double denom = rhs.re_ * rhs.re_ + rhs.im_ * rhs.im_;
    if (denom < EPS) throw std::domain_error("Complex division by zero.");
    return {(re_ * rhs.re_ + im_ * rhs.im_) / denom,
            (im_ * rhs.re_ - re_ * rhs.im_) / denom};
}

Complex& Complex::operator+=(const Complex& rhs) { *this = *this + rhs; return *this; }
Complex& Complex::operator-=(const Complex& rhs) { *this = *this - rhs; return *this; }
Complex& Complex::operator*=(const Complex& rhs) { *this = *this * rhs; return *this; }
Complex& Complex::operator/=(const Complex& rhs) { *this = *this / rhs; return *this; }

Complex Complex::operator-() const { return {-re_, -im_}; }

bool Complex::operator==(const Complex& rhs) const {
    return std::abs(re_ - rhs.re_) < EPS && std::abs(im_ - rhs.im_) < EPS;
}
bool Complex::operator!=(const Complex& rhs) const { return !(*this == rhs); }

Complex Complex::operator+(double s) const { return {re_ + s, im_}; }
Complex Complex::operator-(double s) const { return {re_ - s, im_}; }
Complex Complex::operator*(double s) const { return {re_ * s, im_ * s}; }
Complex Complex::operator/(double s) const {
    if (std::abs(s) < EPS) throw std::domain_error("Division by zero scalar.");
    return {re_ / s, im_ / s};
}

Complex operator+(double s, const Complex& z) { return {s + z.re_, z.im_}; }
Complex operator-(double s, const Complex& z) { return {s - z.re_, -z.im_}; }
Complex operator*(double s, const Complex& z) { return {s * z.re_, s * z.im_}; }

// ── Properties ────────────────────────────────────────────────────────────────

double  Complex::modulus()   const { return std::sqrt(re_ * re_ + im_ * im_); }
double  Complex::modulusSq() const { return re_ * re_ + im_ * im_; }
double  Complex::argument()  const { return std::atan2(im_, re_); }

Complex Complex::conjugate() const { return {re_, -im_}; }

Complex Complex::inverse()   const {
    double d = modulusSq();
    if (d < EPS) throw std::domain_error("Inverse of zero complex number.");
    return {re_ / d, -im_ / d};
}

Complex Complex::normalize() const {
    double m = modulus();
    if (m < EPS) throw std::domain_error("Cannot normalize zero complex number.");
    return {re_ / m, im_ / m};
}

bool Complex::isReal()     const { return std::abs(im_) < EPS; }
bool Complex::isPureImag() const { return std::abs(re_) < EPS && std::abs(im_) >= EPS; }
bool Complex::isZero()     const { return std::abs(re_) < EPS && std::abs(im_) < EPS; }

// ── Algebraic functions ───────────────────────────────────────────────────────

Complex Complex::pow(int n) const {
    // De Moivre's theorem: r^n · (cos nθ + i sin nθ)
    double r   = modulus();
    double arg = argument();
    double rn  = std::pow(r, n);
    return {rn * std::cos(n * arg), rn * std::sin(n * arg)};
}

Complex Complex::pow(const Complex& w) const {
    return (w * this->log()).exp();
}

Complex Complex::sqrt() const {
    double r    = modulus();
    double rSqrt = std::sqrt(r);
    double arg  = argument();
    return {rSqrt * std::cos(arg / 2.0), rSqrt * std::sin(arg / 2.0)};
}

Complex Complex::log() const {
    if (isZero()) throw std::domain_error("log(0) is undefined.");
    return {std::log(modulus()), argument()};
}

Complex Complex::log(const Complex& base) const {
    return this->log() / base.log();
}

Complex Complex::exp() const {
    double ea = std::exp(re_);
    return {ea * std::cos(im_), ea * std::sin(im_)};
}

// ── Trigonometric ─────────────────────────────────────────────────────────────

Complex Complex::sin()  const {
    // sin z = sin(re)cosh(im) + i cos(re)sinh(im)
    return {std::sin(re_) * std::cosh(im_), std::cos(re_) * std::sinh(im_)};
}

Complex Complex::cos()  const {
    return {std::cos(re_) * std::cosh(im_), -std::sin(re_) * std::sinh(im_)};
}

Complex Complex::tan()  const { return sin() / cos(); }
Complex Complex::cot()  const { return cos() / sin(); }
Complex Complex::sec()  const { return Complex::ONE / cos(); }
Complex Complex::csc()  const { return Complex::ONE / sin(); }

// ── Hyperbolic ────────────────────────────────────────────────────────────────

Complex Complex::sinh() const {
    return {std::sinh(re_) * std::cos(im_), std::cosh(re_) * std::sin(im_)};
}
Complex Complex::cosh() const {
    return {std::cosh(re_) * std::cos(im_), std::sinh(re_) * std::sin(im_)};
}
Complex Complex::tanh() const { return sinh() / cosh(); }

// ── Inverse trig ──────────────────────────────────────────────────────────────

Complex Complex::asin() const {
    // asin z = -i · ln(iz + sqrt(1 - z²))
    Complex iz{-im_, re_};
    Complex inner = (Complex::ONE - (*this) * (*this)).sqrt();
    return (Complex::I * -1.0) * (iz + inner).log();
}

Complex Complex::acos() const {
    // acos z = -i · ln(z + i·sqrt(1 - z²))
    Complex inner = (Complex::ONE - (*this) * (*this)).sqrt();
    return (Complex::I * -1.0) * (*this + Complex::I * inner).log();
}

Complex Complex::atan() const {
    // atan z = i/2 · ln((1 - iz)/(1 + iz))
    Complex iz{-im_, re_};
    return (Complex::I * 0.5) * ((Complex::ONE - iz) / (Complex::ONE + iz)).log();
}

// ── Roots ─────────────────────────────────────────────────────────────────────

std::vector<Complex> Complex::nthRoots(int n) const {
    if (n <= 0) throw std::invalid_argument("nthRoots: n must be positive.");
    std::vector<Complex> roots;
    roots.reserve(n);
    double r    = std::pow(modulus(), 1.0 / n);
    double arg  = argument();
    double step = 2.0 * M_PI / n;
    for (int k = 0; k < n; ++k)
        roots.emplace_back(r * std::cos((arg + k * step * 2 * M_PI) / n),
                           r * std::sin((arg + k * step * 2 * M_PI) / n));
    return roots;
}

// ── Representation ────────────────────────────────────────────────────────────

std::pair<double, double> Complex::toPolar() const {
    return {modulus(), argument()};
}

std::string Complex::toString(int precision) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);

    if (std::abs(re_) >= EPS) oss << re_;

    if (std::abs(im_) >= EPS) {
        if (im_ > 0 && std::abs(re_) >= EPS) oss << " + " << im_ << "i";
        else if (im_ < 0)                     oss << " - " << -im_ << "i";
        else                                  oss << im_ << "i";
    } else if (std::abs(re_) < EPS) {
        oss << "0";
    }
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Complex& z) {
    os << z.toString();
    return os;
}

std::istream& operator>>(std::istream& is, Complex& z) {
    double re = 0, im = 0;
    char ch;
    is >> re;
    if (is.peek() == '+' || is.peek() == '-') {
        is >> ch >> im >> ch; // consume 'i'
        if (ch == '-') im = -im;
    }
    z = Complex(re, im);
    return is;
}

// ── Free function helpers ─────────────────────────────────────────────────────

Complex cexp  (const Complex& z) { return z.exp();   }
Complex clog  (const Complex& z) { return z.log();   }
Complex csqrt (const Complex& z) { return z.sqrt();  }
Complex cpow  (const Complex& base, const Complex& e) { return base.pow(e); }
Complex csin  (const Complex& z) { return z.sin();   }
Complex ccos  (const Complex& z) { return z.cos();   }
Complex ctan  (const Complex& z) { return z.tan();   }
Complex csinh (const Complex& z) { return z.sinh();  }
Complex ccosh (const Complex& z) { return z.cosh();  }
Complex ctanh (const Complex& z) { return z.tanh();  }

} // namespace Numerical