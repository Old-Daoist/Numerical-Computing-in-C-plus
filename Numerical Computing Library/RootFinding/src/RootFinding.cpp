#include "../include/RootFinding.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Numerical {

// ── Constructor / configuration ───────────────────────────────────────────────

RootFinder::RootFinder(double tol, int maxIter)
    : tol_(tol), maxIter_(maxIter) {}

void   RootFinder::setTolerance(double tol)    { tol_     = tol; }
void   RootFinder::setMaxIterations(int n)     { maxIter_ = n;   }
double RootFinder::getTolerance()    const     { return tol_;    }
int    RootFinder::getMaxIterations() const    { return maxIter_;}

// ── Bracketing methods ────────────────────────────────────────────────────────

RootResult RootFinder::bisection(std::function<double(double)> f,
                                  double a, double b) const {
    RootResult res{0.0, 0.0, 0, false, "Bisection"};

    if (f(a) * f(b) >= 0.0)
        throw std::invalid_argument("Bisection: f(a) and f(b) must have opposite signs.");

    double mid = a;
    for (int i = 0; i < maxIter_; ++i) {
        mid     = 0.5 * (a + b);
        double fm = f(mid);
        res.iterations = i + 1;

        if (std::abs(fm) < tol_ || (b - a) * 0.5 < tol_) {
            res.converged = true;
            break;
        }
        if (f(a) * fm < 0.0) b = mid;
        else                  a = mid;
    }
    res.root = mid;
    res.fx   = f(mid);
    return res;
}

RootResult RootFinder::regulaFalsi(std::function<double(double)> f,
                                    double a, double b) const {
    RootResult res{0.0, 0.0, 0, false, "Regula Falsi"};

    if (f(a) * f(b) >= 0.0)
        throw std::invalid_argument("Regula Falsi: f(a) and f(b) must have opposite signs.");

    double fa = f(a), fb = f(b), c = a;
    for (int i = 0; i < maxIter_; ++i) {
        c = (a * fb - b * fa) / (fb - fa);
        double fc = f(c);
        res.iterations = i + 1;

        if (std::abs(fc) < tol_) { res.converged = true; break; }
        if (fa * fc < 0.0) { b = c; fb = fc; }
        else               { a = c; fa = fc; }
    }
    res.root = c;
    res.fx   = f(c);
    return res;
}

RootResult RootFinder::illinois(std::function<double(double)> f,
                                 double a, double b) const {
    RootResult res{0.0, 0.0, 0, false, "Illinois"};

    if (f(a) * f(b) >= 0.0)
        throw std::invalid_argument("Illinois: f(a) and f(b) must have opposite signs.");

    double fa = f(a), fb = f(b), c = a;
    int side = 0; // 0 = none, 1 = same-a side, -1 = same-b side

    for (int i = 0; i < maxIter_; ++i) {
        c = (a * fb - b * fa) / (fb - fa);
        double fc = f(c);
        res.iterations = i + 1;

        if (std::abs(fc) < tol_ || std::abs(b - a) < tol_) {
            res.converged = true; break;
        }
        if (fc * fb < 0.0) {
            a = b; fa = fb;
            b = c; fb = fc;
            if (side == 1) fa *= 0.5;
            side = 1;
        } else {
            b = c; fb = fc;
            if (side == -1) fb *= 0.5;
            side = -1;
        }
    }
    res.root = c;
    res.fx   = f(c);
    return res;
}

RootResult RootFinder::brent(std::function<double(double)> f,
                              double a, double b) const {
    RootResult res{0.0, 0.0, 0, false, "Brent"};

    double fa = f(a), fb = f(b);
    if (fa * fb >= 0.0)
        throw std::invalid_argument("Brent: f(a) and f(b) must have opposite signs.");

    if (std::abs(fa) < std::abs(fb)) { std::swap(a, b); std::swap(fa, fb); }

    double c = a, fc = fa, s = b, fs = fb;
    double d = b - a;
    bool mflag = true;

    for (int i = 0; i < maxIter_; ++i) {
        if (std::abs(fb) < tol_ || std::abs(b - a) < tol_) {
            res.converged = true; break;
        }
        // Inverse-quadratic interpolation or Secant
        if (fa != fc && fb != fc) {
            s = a * fb * fc / ((fa - fb) * (fa - fc))
              + b * fa * fc / ((fb - fa) * (fb - fc))
              + c * fa * fb / ((fc - fa) * (fc - fb));
        } else {
            s = b - fb * (b - a) / (fb - fa);
        }
        // Decide whether to use bisection instead
        double mid = 0.5 * (a + b);
        bool cond1 = (s < (3.0 * a + b) / 4.0) || (s > b);
        bool cond2 = mflag && std::abs(s - b) >= 0.5 * std::abs(b - c);
        bool cond3 = !mflag && std::abs(s - b) >= 0.5 * std::abs(c - d);
        bool cond4 = mflag && std::abs(b - c) < tol_;
        bool cond5 = !mflag && std::abs(c - d) < tol_;

        if (cond1 || cond2 || cond3 || cond4 || cond5) {
            s = mid; mflag = true;
        } else { mflag = false; }

        fs = f(s);
        d  = c; c = b; fc = fb;
        if (fa * fs < 0.0) { b = s; fb = fs; }
        else               { a = s; fa = fs; }

        if (std::abs(fa) < std::abs(fb)) { std::swap(a, b); std::swap(fa, fb); }
        res.iterations = i + 1;
    }
    res.root = b;
    res.fx   = fb;
    return res;
}

// ── Open methods ──────────────────────────────────────────────────────────────

RootResult RootFinder::newton(std::function<double(double)> f,
                               std::function<double(double)> df,
                               double x0) const {
    RootResult res{x0, 0.0, 0, false, "Newton-Raphson"};

    double x = x0;
    for (int i = 0; i < maxIter_; ++i) {
        double fx  = f(x);
        double dfx = df(x);
        if (std::abs(dfx) < 1e-14)
            throw std::runtime_error("Newton: derivative near zero — possible stationary point.");

        double xNew = x - fx / dfx;
        res.iterations = i + 1;

        if (std::abs(xNew - x) < tol_ && std::abs(f(xNew)) < tol_) {
            res.converged = true;
            x = xNew; break;
        }
        x = xNew;
    }
    res.root = x;
    res.fx   = f(x);
    return res;
}

RootResult RootFinder::halley(std::function<double(double)> f,
                               std::function<double(double)> df,
                               std::function<double(double)> d2f,
                               double x0) const {
    RootResult res{x0, 0.0, 0, false, "Halley"};

    double x = x0;
    for (int i = 0; i < maxIter_; ++i) {
        double fx   = f(x);
        double dfx  = df(x);
        double d2fx = d2f(x);
        double denom = 2.0 * dfx * dfx - fx * d2fx;

        if (std::abs(denom) < 1e-14)
            throw std::runtime_error("Halley: denominator near zero.");

        double xNew = x - (2.0 * fx * dfx) / denom;
        res.iterations = i + 1;

        if (std::abs(xNew - x) < tol_) { res.converged = true; x = xNew; break; }
        x = xNew;
    }
    res.root = x;
    res.fx   = f(x);
    return res;
}

RootResult RootFinder::secant(std::function<double(double)> f,
                               double x0, double x1) const {
    RootResult res{x1, 0.0, 0, false, "Secant"};

    double xPrev = x0, xCurr = x1;
    double fPrev = f(x0), fCurr = f(x1);

    for (int i = 0; i < maxIter_; ++i) {
        double denom = fCurr - fPrev;
        if (std::abs(denom) < 1e-14)
            throw std::runtime_error("Secant: divided difference near zero.");

        double xNew = xCurr - fCurr * (xCurr - xPrev) / denom;
        res.iterations = i + 1;

        if (std::abs(xNew - xCurr) < tol_) { res.converged = true; xCurr = xNew; break; }
        xPrev = xCurr; fPrev = fCurr;
        xCurr = xNew;  fCurr = f(xNew);
    }
    res.root = xCurr;
    res.fx   = f(xCurr);
    return res;
}

RootResult RootFinder::fixedPoint(std::function<double(double)> g,
                                   double x0) const {
    RootResult res{x0, 0.0, 0, false, "Fixed-Point"};

    double x = x0;
    for (int i = 0; i < maxIter_; ++i) {
        double xNew = g(x);
        res.iterations = i + 1;

        if (std::abs(xNew - x) < tol_) { res.converged = true; x = xNew; break; }
        x = xNew;
    }
    res.root = x;
    res.fx   = g(x) - x;   // residual of fixed-point equation
    return res;
}

RootResult RootFinder::steffensen(std::function<double(double)> g,
                                   double x0) const {
    RootResult res{x0, 0.0, 0, false, "Steffensen"};

    double x = x0;
    for (int i = 0; i < maxIter_; ++i) {
        double gx  = g(x);
        double ggx = g(gx);
        double denom = ggx - 2.0 * gx + x;
        if (std::abs(denom) < 1e-14)
            throw std::runtime_error("Steffensen: Aitken denominator near zero.");

        double xNew = x - (gx - x) * (gx - x) / denom;
        res.iterations = i + 1;

        if (std::abs(xNew - x) < tol_) { res.converged = true; x = xNew; break; }
        x = xNew;
    }
    res.root = x;
    res.fx   = g(x) - x;
    return res;
}

RootResult RootFinder::ridder(std::function<double(double)> f,
                               double a, double b) const {
    RootResult res{0.0, 0.0, 0, false, "Ridder"};

    double fa = f(a), fb = f(b);
    if (fa * fb >= 0.0)
        throw std::invalid_argument("Ridder: f(a) and f(b) must have opposite signs.");

    double xNew = a;
    for (int i = 0; i < maxIter_; ++i) {
        double mid = 0.5 * (a + b);
        double fm  = f(mid);
        double sign = (fa - fb < 0) ? -1.0 : 1.0;
        double denom = std::sqrt(fm * fm - fa * fb);
        if (std::abs(denom) < 1e-14) { res.converged = true; break; }

        xNew = mid + (mid - a) * sign * fm / denom;
        double fNew = f(xNew);
        res.iterations = i + 1;

        if (std::abs(fNew) < tol_ || std::abs(b - a) < tol_) {
            res.converged = true; break;
        }
        if (fm * fNew < 0.0) { a = mid; fa = fm; b = xNew; fb = fNew; }
        else if (fa * fNew < 0.0) { b = xNew; fb = fNew; }
        else                       { a = xNew; fa = fNew; }
    }
    res.root = xNew;
    res.fx   = f(xNew);
    return res;
}

// ── Utility ───────────────────────────────────────────────────────────────────

void RootFinder::printResult(const RootResult& r) {
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "┌─ " << r.method << " ─────────────────────────────\n";
    std::cout << "│  Root       : " << r.root       << "\n";
    std::cout << "│  f(root)    : " << r.fx         << "\n";
    std::cout << "│  Iterations : " << r.iterations << "\n";
    std::cout << "│  Converged  : " << (r.converged ? "Yes" : "No") << "\n";
    std::cout << "└─────────────────────────────────────────────\n";
}

} // namespace Numerical