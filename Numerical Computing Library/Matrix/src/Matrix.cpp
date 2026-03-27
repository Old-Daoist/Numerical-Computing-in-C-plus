#include "../include/Matrix.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <random>
#include <numeric>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Numerical {

// ══════════════════════════════════════════════════════════════════════════════
//  Constructors
// ══════════════════════════════════════════════════════════════════════════════

Matrix::Matrix() : rows_(0), cols_(0) {}

Matrix::Matrix(int rows, int cols, double init)
    : rows_(rows), cols_(cols), data_(rows * cols, init) {}

Matrix::Matrix(int n, double init)
    : rows_(n), cols_(n), data_(n * n, init) {}

Matrix::Matrix(std::initializer_list<std::initializer_list<double>> lst) {
    rows_ = static_cast<int>(lst.size());
    cols_ = rows_ ? static_cast<int>(lst.begin()->size()) : 0;
    data_.reserve(rows_ * cols_);
    for (auto& row : lst)
        for (double v : row)
            data_.push_back(v);
}

Matrix::Matrix(const std::vector<std::vector<double>>& v) {
    rows_ = static_cast<int>(v.size());
    cols_ = rows_ ? static_cast<int>(v[0].size()) : 0;
    data_.reserve(rows_ * cols_);
    for (auto& row : v)
        for (double val : row)
            data_.push_back(val);
}

// ══════════════════════════════════════════════════════════════════════════════
//  Factory methods
// ══════════════════════════════════════════════════════════════════════════════

Matrix Matrix::identity(int n) {
    Matrix M(n, n, 0.0);
    for (int i = 0; i < n; ++i) M(i, i) = 1.0;
    return M;
}

Matrix Matrix::zeros(int r, int c)  { return Matrix(r, c, 0.0); }
Matrix Matrix::ones (int r, int c)  { return Matrix(r, c, 1.0); }

Matrix Matrix::diagonal(const std::vector<double>& d) {
    int n = static_cast<int>(d.size());
    Matrix M(n, n, 0.0);
    for (int i = 0; i < n; ++i) M(i, i) = d[i];
    return M;
}

Matrix Matrix::random(int r, int c, double lo, double hi) {
    Matrix M(r, c);
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(lo, hi);
    for (auto& v : M.data_) v = dist(rng);
    return M;
}

Matrix Matrix::hilbert(int n) {
    Matrix M(n, n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            M(i, j) = 1.0 / (i + j + 1);
    return M;
}

Matrix Matrix::companion(const std::vector<double>& coeffs) {
    // coeffs = {a0, a1, ..., a_{n-1}} for p(x) = xⁿ + a_{n-1}x^{n-1} + ... + a0
    int n = static_cast<int>(coeffs.size());
    Matrix C(n, n, 0.0);
    for (int i = 1; i < n; ++i) C(i, i - 1) = 1.0;
    for (int i = 0; i < n; ++i) C(i, n - 1) = -coeffs[i];
    return C;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Element access
// ══════════════════════════════════════════════════════════════════════════════

double&       Matrix::operator()(int r, int c)       { return data_[r * cols_ + c]; }
const double& Matrix::operator()(int r, int c) const { return data_[r * cols_ + c]; }
double&       Matrix::at(int r, int c)               { return data_[r * cols_ + c]; }
const double& Matrix::at(int r, int c)         const { return data_[r * cols_ + c]; }

int  Matrix::rows()     const { return rows_; }
int  Matrix::cols()     const { return cols_; }
bool Matrix::isSquare() const { return rows_ == cols_; }

std::vector<double> Matrix::getRow(int r) const {
    return std::vector<double>(data_.begin() + r * cols_,
                               data_.begin() + r * cols_ + cols_);
}

std::vector<double> Matrix::getCol(int c) const {
    std::vector<double> col(rows_);
    for (int i = 0; i < rows_; ++i) col[i] = at(i, c);
    return col;
}

void Matrix::setRow(int r, const std::vector<double>& v) {
    for (int j = 0; j < cols_; ++j) at(r, j) = v[j];
}

void Matrix::setCol(int c, const std::vector<double>& v) {
    for (int i = 0; i < rows_; ++i) at(i, c) = v[i];
}

Matrix Matrix::submatrix(int r0, int r1, int c0, int c1) const {
    Matrix S(r1 - r0, c1 - c0);
    for (int i = r0; i < r1; ++i)
        for (int j = c0; j < c1; ++j)
            S(i - r0, j - c0) = at(i, j);
    return S;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Arithmetic
// ══════════════════════════════════════════════════════════════════════════════

Matrix Matrix::operator+(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_)
        throw std::invalid_argument("Matrix addition: size mismatch.");
    Matrix R(rows_, cols_);
    for (int i = 0; i < (int)data_.size(); ++i)
        R.data_[i] = data_[i] + rhs.data_[i];
    return R;
}

Matrix Matrix::operator-(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_)
        throw std::invalid_argument("Matrix subtraction: size mismatch.");
    Matrix R(rows_, cols_);
    for (int i = 0; i < (int)data_.size(); ++i)
        R.data_[i] = data_[i] - rhs.data_[i];
    return R;
}

Matrix Matrix::operator*(const Matrix& rhs) const {
    if (cols_ != rhs.rows_)
        throw std::invalid_argument("Matrix multiplication: inner dimensions mismatch.");
    Matrix R(rows_, rhs.cols_, 0.0);
    for (int i = 0; i < rows_; ++i)
        for (int k = 0; k < cols_; ++k)
            for (int j = 0; j < rhs.cols_; ++j)
                R(i, j) += at(i, k) * rhs(k, j);
    return R;
}

Matrix Matrix::operator*(double s) const {
    Matrix R(rows_, cols_);
    for (int i = 0; i < (int)data_.size(); ++i) R.data_[i] = data_[i] * s;
    return R;
}

Matrix Matrix::operator/(double s) const {
    if (std::abs(s) < EPS) throw std::domain_error("Matrix / 0.");
    return *this * (1.0 / s);
}

Matrix Matrix::operator-() const {
    Matrix R(rows_, cols_);
    for (int i = 0; i < (int)data_.size(); ++i) R.data_[i] = -data_[i];
    return R;
}

Matrix& Matrix::operator+=(const Matrix& rhs) { *this = *this + rhs; return *this; }
Matrix& Matrix::operator-=(const Matrix& rhs) { *this = *this - rhs; return *this; }
Matrix& Matrix::operator*=(double s)          { for (auto& v : data_) v *= s; return *this; }

bool Matrix::operator==(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_) return false;
    for (int i = 0; i < (int)data_.size(); ++i)
        if (std::abs(data_[i] - rhs.data_[i]) > EPS) return false;
    return true;
}

Matrix operator*(double s, const Matrix& M) { return M * s; }

Matrix Matrix::hadamard(const Matrix& rhs) const {
    if (rows_ != rhs.rows_ || cols_ != rhs.cols_)
        throw std::invalid_argument("Hadamard: size mismatch.");
    Matrix R(rows_, cols_);
    for (int i = 0; i < (int)data_.size(); ++i)
        R.data_[i] = data_[i] * rhs.data_[i];
    return R;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Basic operations
// ══════════════════════════════════════════════════════════════════════════════

Matrix Matrix::transpose() const {
    Matrix T(cols_, rows_);
    for (int i = 0; i < rows_; ++i)
        for (int j = 0; j < cols_; ++j)
            T(j, i) = at(i, j);
    return T;
}

double Matrix::trace() const {
    if (!isSquare()) throw std::invalid_argument("Trace requires a square matrix.");
    double t = 0;
    for (int i = 0; i < rows_; ++i) t += at(i, i);
    return t;
}

double Matrix::frobenius() const {
    double s = 0;
    for (double v : data_) s += v * v;
    return std::sqrt(s);
}

double Matrix::norm1() const {   // max column sum
    double n = 0;
    for (int j = 0; j < cols_; ++j) {
        double s = 0;
        for (int i = 0; i < rows_; ++i) s += std::abs(at(i, j));
        n = std::max(n, s);
    }
    return n;
}

double Matrix::normInf() const { // max row sum
    double n = 0;
    for (int i = 0; i < rows_; ++i) {
        double s = 0;
        for (int j = 0; j < cols_; ++j) s += std::abs(at(i, j));
        n = std::max(n, s);
    }
    return n;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Matrix properties
// ══════════════════════════════════════════════════════════════════════════════

bool Matrix::isSymmetric(double tol) const {
    if (!isSquare()) return false;
    for (int i = 0; i < rows_; ++i)
        for (int j = i + 1; j < cols_; ++j)
            if (std::abs(at(i, j) - at(j, i)) > tol) return false;
    return true;
}

bool Matrix::isOrthogonal(double tol) const {
    if (!isSquare()) return false;
    Matrix QtQ = transpose() * (*this);
    Matrix I   = identity(rows_);
    return (QtQ - I).frobenius() < tol;
}

bool Matrix::isDiagonallyDominant() const {
    if (!isSquare()) return false;
    for (int i = 0; i < rows_; ++i) {
        double diag = std::abs(at(i, i));
        double rest = 0;
        for (int j = 0; j < cols_; ++j)
            if (j != i) rest += std::abs(at(i, j));
        if (diag < rest) return false;
    }
    return true;
}

bool Matrix::isPositiveDefinite() const {
    if (!isSymmetric()) return false;
    try { choleskyDecomposition(); return true; }
    catch (...) { return false; }
}

bool Matrix::isSingular(double tol) const {
    return std::abs(determinant()) < tol;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Internal helpers
// ══════════════════════════════════════════════════════════════════════════════

void Matrix::swapRows(int r1, int r2) {
    for (int j = 0; j < cols_; ++j)
        std::swap(at(r1, j), at(r2, j));
}

Matrix Matrix::forwardSubstitution(const Matrix& L, const Matrix& b) const {
    int n = L.rows_;
    Matrix x(n, 1, 0.0);
    for (int i = 0; i < n; ++i) {
        double s = b(i, 0);
        for (int j = 0; j < i; ++j) s -= L(i, j) * x(j, 0);
        x(i, 0) = s / L(i, i);
    }
    return x;
}

Matrix Matrix::backSubstitution(const Matrix& U, const Matrix& b) const {
    int n = U.rows_;
    Matrix x(n, 1, 0.0);
    for (int i = n - 1; i >= 0; --i) {
        double s = b(i, 0);
        for (int j = i + 1; j < n; ++j) s -= U(i, j) * x(j, 0);
        x(i, 0) = s / U(i, i);
    }
    return x;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Decompositions
// ══════════════════════════════════════════════════════════════════════════════

std::tuple<Matrix, Matrix, Matrix> Matrix::luDecomposition() const {
    if (!isSquare())
        throw std::invalid_argument("LU Decomposition requires a square matrix.");
    int n = rows_;
    Matrix L = identity(n);
    Matrix U = *this;
    Matrix P = identity(n);

    for (int k = 0; k < n; ++k) {
        // Partial pivoting
        int    pivot  = k;
        double maxVal = std::abs(U(k, k));
        for (int i = k + 1; i < n; ++i)
            if (std::abs(U(i, k)) > maxVal) { maxVal = std::abs(U(i, k)); pivot = i; }

        if (maxVal < EPS)
            throw std::runtime_error("LU: Matrix is (near-)singular.");

        if (pivot != k) {
            U.swapRows(k, pivot);
            P.swapRows(k, pivot);
            for (int j = 0; j < k; ++j) std::swap(L(k, j), L(pivot, j));
        }
        for (int i = k + 1; i < n; ++i) {
            double factor = U(i, k) / U(k, k);
            L(i, k) = factor;
            for (int j = k; j < n; ++j)
                U(i, j) -= factor * U(k, j);
        }
    }
    return {L, U, P};
}

std::pair<Matrix, Matrix> Matrix::ldltDecomposition() const {
    if (!isSymmetric())
        throw std::invalid_argument("LDLt: Matrix must be symmetric.");
    int n = rows_;
    Matrix L = identity(n);
    Matrix D = zeros(n, n);

    for (int j = 0; j < n; ++j) {
        double d = at(j, j);
        for (int k = 0; k < j; ++k) d -= L(j, k) * L(j, k) * D(k, k);
        D(j, j) = d;
        if (std::abs(d) < EPS)
            throw std::runtime_error("LDLt: Zero diagonal encountered (matrix may not be positive definite).");
        for (int i = j + 1; i < n; ++i) {
            double s = at(i, j);
            for (int k = 0; k < j; ++k) s -= L(i, k) * L(j, k) * D(k, k);
            L(i, j) = s / d;
        }
    }
    return {L, D};
}

Matrix Matrix::choleskyDecomposition() const {
    if (!isSymmetric())
        throw std::invalid_argument("Cholesky: Matrix must be symmetric.");
    int n = rows_;
    Matrix L(n, n, 0.0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= i; ++j) {
            double s = at(i, j);
            for (int k = 0; k < j; ++k) s -= L(i, k) * L(j, k);
            if (i == j) {
                if (s <= 0) throw std::runtime_error("Cholesky: Matrix is not positive definite.");
                L(i, j) = std::sqrt(s);
            } else {
                L(i, j) = s / L(j, j);
            }
        }
    }
    return L;
}

std::pair<Matrix, Matrix> Matrix::qrDecomposition() const {
    int m = rows_, n = cols_;
    Matrix Q = identity(m);
    Matrix R = *this;

    for (int k = 0; k < std::min(m - 1, n); ++k) {
        // Householder vector for column k, rows k..m-1
        Matrix x(m - k, 1, 0.0);
        for (int i = k; i < m; ++i) x(i - k, 0) = R(i, k);

        double sigma = 0;
        for (int i = 0; i < x.rows_; ++i) sigma += x(i, 0) * x(i, 0);
        double norm_x = std::sqrt(sigma);

        x(0, 0) += (x(0, 0) >= 0 ? 1.0 : -1.0) * norm_x;
        double norm_v2 = 0;
        for (int i = 0; i < x.rows_; ++i) norm_v2 += x(i, 0) * x(i, 0);

        if (norm_v2 < EPS) continue;

        // Apply H = I - 2vvᵀ/‖v‖² to R and Q
        for (int j = k; j < n; ++j) {
            double dot = 0;
            for (int i = k; i < m; ++i) dot += x(i - k, 0) * R(i, j);
            dot *= 2.0 / norm_v2;
            for (int i = k; i < m; ++i) R(i, j) -= dot * x(i - k, 0);
        }
        for (int j = 0; j < m; ++j) {
            double dot = 0;
            for (int i = k; i < m; ++i) dot += x(i - k, 0) * Q(i, j);
            dot *= 2.0 / norm_v2;
            for (int i = k; i < m; ++i) Q(i, j) -= dot * x(i - k, 0);
        }
    }
    return {Q.transpose(), R};
}

std::tuple<Matrix, Matrix, Matrix> Matrix::svd() const {
    // Golub-Reinsch via QR iteration on AᵀA
    // For a full production SVD see LAPACK; this covers the common case.
    int m = rows_, n = cols_;
    Matrix U  = identity(m);
    Matrix Vt = identity(n);
    Matrix B  = *this;

    // Bidiagonalize B = U1 * bidiag * Vt1 using Householder
    Matrix AtA = transpose() * (*this);
    auto [eigVals, eigVecs] = AtA.jacobiEigen(1e-10, 200);

    // Singular values = sqrt(eigenvalues of AᵀA)
    int k = std::min(m, n);
    Matrix Sigma = zeros(m, n);
    for (int i = 0; i < k; ++i) {
        double sv = (eigVals[i] > 0) ? std::sqrt(eigVals[i]) : 0.0;
        Sigma(i, i) = sv;
    }

    // V columns = eigenvectors of AᵀA
    Matrix V = eigVecs;

    // U columns: u_i = A·v_i / σ_i
    for (int i = 0; i < k; ++i) {
        if (Sigma(i, i) > EPS) {
            Matrix v_col(n, 1);
            for (int r = 0; r < n; ++r) v_col(r, 0) = V(r, i);
            Matrix u_col = (*this) * v_col / Sigma(i, i);
            for (int r = 0; r < m; ++r) U(r, i) = u_col(r, 0);
        }
    }

    return {U, Sigma, V.transpose()};
}

std::pair<Matrix, Matrix> Matrix::hessenbergReduction() const {
    if (!isSquare()) throw std::invalid_argument("Hessenberg: requires square matrix.");
    int n = rows_;
    Matrix H = *this;
    Matrix Q = identity(n);

    for (int k = 0; k < n - 2; ++k) {
        Matrix x(n - k - 1, 1, 0.0);
        for (int i = k + 1; i < n; ++i) x(i - k - 1, 0) = H(i, k);

        double norm = 0;
        for (int i = 0; i < x.rows_; ++i) norm += x(i, 0) * x(i, 0);
        norm = std::sqrt(norm);
        if (norm < EPS) continue;

        x(0, 0) += (x(0, 0) >= 0 ? 1.0 : -1.0) * norm;
        double norm_v2 = 0;
        for (int i = 0; i < x.rows_; ++i) norm_v2 += x(i, 0) * x(i, 0);

        // H = (I - 2vvᵀ) H (I - 2vvᵀ)
        for (int j = 0; j < n; ++j) {
            double dot = 0;
            for (int i = k + 1; i < n; ++i) dot += x(i - k - 1, 0) * H(i, j);
            dot *= 2.0 / norm_v2;
            for (int i = k + 1; i < n; ++i) H(i, j) -= dot * x(i - k - 1, 0);
        }
        for (int i = 0; i < n; ++i) {
            double dot = 0;
            for (int j = k + 1; j < n; ++j) dot += H(i, j) * x(j - k - 1, 0);
            dot *= 2.0 / norm_v2;
            for (int j = k + 1; j < n; ++j) H(i, j) -= dot * x(j - k - 1, 0);
        }
        for (int i = 0; i < n; ++i) {
            double dot = 0;
            for (int j = k + 1; j < n; ++j) dot += Q(i, j) * x(j - k - 1, 0);
            dot *= 2.0 / norm_v2;
            for (int j = k + 1; j < n; ++j) Q(i, j) -= dot * x(j - k - 1, 0);
        }
    }
    return {Q, H};
}

std::pair<Matrix, Matrix> Matrix::schurDecomposition() const {
    if (!isSquare()) throw std::invalid_argument("Schur: requires square matrix.");
    int n = rows_;
    auto [Q0, T] = hessenbergReduction();
    Matrix Q = Q0;

    for (int iter = 0; iter < 1000; ++iter) {
        double shift = T(n - 1, n - 1);
        Matrix Tshift = T;
        for (int i = 0; i < n; ++i) Tshift(i, i) -= shift;
        auto [Qi, Ri] = Tshift.qrDecomposition();
        T = Ri * Qi;
        for (int i = 0; i < n; ++i) T(i, i) += shift;
        Q = Q * Qi;

        double offDiag = 0;
        for (int i = 0; i < n - 1; ++i) offDiag += T(i + 1, i) * T(i + 1, i);
        if (std::sqrt(offDiag) < EPS) break;
    }
    return {Q, T};
}

// ══════════════════════════════════════════════════════════════════════════════
//  Solve linear systems
// ══════════════════════════════════════════════════════════════════════════════

Matrix Matrix::solve(const Matrix& b) const {
    auto [L, U, P] = luDecomposition();
    Matrix Pb = P * b;
    Matrix y  = forwardSubstitution(L, Pb);
    return backSubstitution(U, y);
}

Matrix Matrix::solveSPD(const Matrix& b) const {
    Matrix L  = choleskyDecomposition();
    Matrix y  = forwardSubstitution(L, b);
    return backSubstitution(L.transpose(), y);
}

Matrix Matrix::solveLeastSquares(const Matrix& b) const {
    auto [Q, R] = qrDecomposition();
    Matrix Qtb = Q.transpose() * b;
    return backSubstitution(R.submatrix(0, cols_, 0, cols_),
                            Qtb.submatrix(0, cols_, 0, 1));
}

Matrix Matrix::gaussianElimination(const Matrix& b) const {
    int n = rows_;
    // Augmented matrix [A | b]
    Matrix Aug(n, n + 1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) Aug(i, j) = at(i, j);
        Aug(i, n) = b(i, 0);
    }
    for (int k = 0; k < n; ++k) {
        // Partial pivot
        int pivot = k;
        for (int i = k + 1; i < n; ++i)
            if (std::abs(Aug(i, k)) > std::abs(Aug(pivot, k))) pivot = i;
        Aug.swapRows(k, pivot);
        if (std::abs(Aug(k, k)) < EPS)
            throw std::runtime_error("Gaussian elimination: singular matrix.");
        for (int i = k + 1; i < n; ++i) {
            double f = Aug(i, k) / Aug(k, k);
            for (int j = k; j <= n; ++j) Aug(i, j) -= f * Aug(k, j);
        }
    }
    // Back substitution
    Matrix x(n, 1);
    for (int i = n - 1; i >= 0; --i) {
        x(i, 0) = Aug(i, n);
        for (int j = i + 1; j < n; ++j) x(i, 0) -= Aug(i, j) * x(j, 0);
        x(i, 0) /= Aug(i, i);
    }
    return x;
}

Matrix Matrix::gaussJordan(const Matrix& b) const {
    int n = rows_;
    Matrix Aug(n, n + 1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) Aug(i, j) = at(i, j);
        Aug(i, n) = b(i, 0);
    }
    for (int k = 0; k < n; ++k) {
        int pivot = k;
        for (int i = k + 1; i < n; ++i)
            if (std::abs(Aug(i, k)) > std::abs(Aug(pivot, k))) pivot = i;
        Aug.swapRows(k, pivot);
        double diag = Aug(k, k);
        if (std::abs(diag) < EPS)
            throw std::runtime_error("Gauss-Jordan: singular matrix.");
        for (int j = k; j <= n; ++j) Aug(k, j) /= diag;
        for (int i = 0; i < n; ++i) {
            if (i == k) continue;
            double f = Aug(i, k);
            for (int j = k; j <= n; ++j) Aug(i, j) -= f * Aug(k, j);
        }
    }
    Matrix x(n, 1);
    for (int i = 0; i < n; ++i) x(i, 0) = Aug(i, n);
    return x;
}

std::vector<double> Matrix::thomasAlgorithm(
    const std::vector<double>& lower,
    const std::vector<double>& main_d,
    const std::vector<double>& upper,
    const std::vector<double>& rhs)
{
    int n = static_cast<int>(main_d.size());
    std::vector<double> c(n), d(n), x(n);

    c[0] = upper[0] / main_d[0];
    d[0] = rhs[0]   / main_d[0];

    for (int i = 1; i < n; ++i) {
        double m = main_d[i] - lower[i - 1] * c[i - 1];
        if (std::abs(m) < 1e-14)
            throw std::runtime_error("Thomas algorithm: zero pivot.");
        c[i] = (i < n - 1) ? upper[i] / m : 0.0;
        d[i] = (rhs[i] - lower[i - 1] * d[i - 1]) / m;
    }
    x[n - 1] = d[n - 1];
    for (int i = n - 2; i >= 0; --i) x[i] = d[i] - c[i] * x[i + 1];
    return x;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Iterative solvers
// ══════════════════════════════════════════════════════════════════════════════

Matrix Matrix::jacobiIteration(const Matrix& b, Matrix x0,
                                double tol, int maxIt) const {
    int n = rows_;
    if (x0.rows_ == 0) x0 = Matrix(n, 1, 0.0);
    Matrix x = x0;

    for (int it = 0; it < maxIt; ++it) {
        Matrix xNew(n, 1);
        for (int i = 0; i < n; ++i) {
            double s = b(i, 0);
            for (int j = 0; j < n; ++j)
                if (j != i) s -= at(i, j) * x(j, 0);
            xNew(i, 0) = s / at(i, i);
        }
        double err = (xNew - x).frobenius();
        x = xNew;
        if (err < tol) break;
    }
    return x;
}

Matrix Matrix::gaussSeidelIteration(const Matrix& b, Matrix x0,
                                     double tol, int maxIt) const {
    int n = rows_;
    if (x0.rows_ == 0) x0 = Matrix(n, 1, 0.0);
    Matrix x = x0;

    for (int it = 0; it < maxIt; ++it) {
        Matrix xOld = x;
        for (int i = 0; i < n; ++i) {
            double s = b(i, 0);
            for (int j = 0; j < n; ++j)
                if (j != i) s -= at(i, j) * x(j, 0);
            x(i, 0) = s / at(i, i);
        }
        if ((x - xOld).frobenius() < tol) break;
    }
    return x;
}

Matrix Matrix::sor(const Matrix& b, double omega, Matrix x0,
                   double tol, int maxIt) const {
    int n = rows_;
    if (x0.rows_ == 0) x0 = Matrix(n, 1, 0.0);
    Matrix x = x0;

    for (int it = 0; it < maxIt; ++it) {
        Matrix xOld = x;
        for (int i = 0; i < n; ++i) {
            double gs = b(i, 0);
            for (int j = 0; j < n; ++j)
                if (j != i) gs -= at(i, j) * x(j, 0);
            gs /= at(i, i);
            x(i, 0) = (1.0 - omega) * x(i, 0) + omega * gs;
        }
        if ((x - xOld).frobenius() < tol) break;
    }
    return x;
}

Matrix Matrix::conjugateGradient(const Matrix& b, Matrix x0,
                                  double tol, int maxIt) const {
    int n = rows_;
    if (x0.rows_ == 0) x0 = Matrix(n, 1, 0.0);
    Matrix x = x0;
    Matrix r = b - (*this) * x;
    Matrix p = r;
    double rsold = (r.transpose() * r)(0, 0);

    for (int it = 0; it < maxIt; ++it) {
        Matrix Ap  = (*this) * p;
        double dot = (p.transpose() * Ap)(0, 0);
        if (std::abs(dot) < EPS) break;
        double alpha = rsold / dot;
        x = x + p * alpha;
        r = r - Ap * alpha;
        double rsnew = (r.transpose() * r)(0, 0);
        if (std::sqrt(rsnew) < tol) break;
        double beta = rsnew / rsold;
        p = r + p * beta;
        rsold = rsnew;
    }
    return x;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Eigenvalues & vectors
// ══════════════════════════════════════════════════════════════════════════════

std::pair<double, Matrix> Matrix::powerIteration(double tol, int maxIt) const {
    if (!isSquare()) throw std::invalid_argument("Power iteration: requires square matrix.");
    int n = rows_;
    Matrix v = random(n, 1);
    double norm = v.frobenius();
    v = v / norm;
    double lambda = 0;

    for (int it = 0; it < maxIt; ++it) {
        Matrix Av = (*this) * v;
        double lambdaNew = (v.transpose() * Av)(0, 0);
        norm = Av.frobenius();
        if (norm < EPS) break;
        v = Av / norm;
        if (std::abs(lambdaNew - lambda) < tol) { lambda = lambdaNew; break; }
        lambda = lambdaNew;
    }
    return {lambda, v};
}

std::pair<double, Matrix> Matrix::inverseIteration(double mu, double tol, int maxIt) const {
    int n = rows_;
    Matrix A_shift = *this;
    for (int i = 0; i < n; ++i) A_shift(i, i) -= mu;

    Matrix v = random(n, 1);
    v = v / v.frobenius();
    double lambda = mu;

    for (int it = 0; it < maxIt; ++it) {
        Matrix w = A_shift.solve(v);
        double norm = w.frobenius();
        if (norm < EPS) break;
        Matrix vNew = w / norm;
        double lambdaNew = (vNew.transpose() * (*this) * vNew)(0, 0);
        if (std::abs(lambdaNew - lambda) < tol) { v = vNew; lambda = lambdaNew; break; }
        v = vNew; lambda = lambdaNew;
    }
    return {lambda, v};
}

std::vector<double> Matrix::eigenvaluesQR(double tol, int maxIt) const {
    if (!isSquare()) throw std::invalid_argument("eigenvaluesQR: requires square matrix.");
    int n = rows_;
    Matrix T = *this;

    for (int it = 0; it < maxIt; ++it) {
        double shift = T(n - 1, n - 1);
        for (int i = 0; i < n; ++i) T(i, i) -= shift;
        auto [Q, R] = T.qrDecomposition();
        T = R * Q;
        for (int i = 0; i < n; ++i) T(i, i) += shift;

        double off = 0;
        for (int i = 0; i < n - 1; ++i) off += T(i + 1, i) * T(i + 1, i);
        if (std::sqrt(off) < tol) break;
    }
    std::vector<double> eigs(n);
    for (int i = 0; i < n; ++i) eigs[i] = T(i, i);
    std::sort(eigs.rbegin(), eigs.rend());
    return eigs;
}

std::pair<std::vector<double>, Matrix> Matrix::jacobiEigen(double tol, int maxIt) const {
    if (!isSymmetric())
        throw std::invalid_argument("Jacobi eigenvalue: requires symmetric matrix.");
    int n = rows_;
    Matrix A = *this;
    Matrix V = identity(n);

    for (int it = 0; it < maxIt; ++it) {
        // Find off-diagonal element with max abs value
        int p = 0, q = 1;
        double maxOff = 0;
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
                if (std::abs(A(i, j)) > maxOff) { maxOff = std::abs(A(i, j)); p = i; q = j; }

        if (maxOff < tol) break;

        // Compute rotation angle
        double theta = 0.5 * std::atan2(2.0 * A(p, q), A(q, q) - A(p, p));
        double c = std::cos(theta), s = std::sin(theta);

        // Build Givens rotation G
        Matrix G = identity(n);
        G(p, p) =  c; G(p, q) = -s;
        G(q, p) =  s; G(q, q) =  c;

        A = G.transpose() * A * G;
        V = V * G;
    }
    std::vector<double> eigs(n);
    for (int i = 0; i < n; ++i) eigs[i] = A(i, i);
    return {eigs, V};
}

// ══════════════════════════════════════════════════════════════════════════════
//  Miscellaneous
// ══════════════════════════════════════════════════════════════════════════════

double Matrix::determinant() const {
    if (!isSquare()) throw std::invalid_argument("Determinant: requires square matrix.");
    int n = rows_;
    Matrix U = *this;
    double det = 1.0;
    int swaps = 0;

    for (int k = 0; k < n; ++k) {
        int pivot = k;
        for (int i = k + 1; i < n; ++i)
            if (std::abs(U(i, k)) > std::abs(U(pivot, k))) pivot = i;
        if (pivot != k) { U.swapRows(k, pivot); ++swaps; }
        if (std::abs(U(k, k)) < EPS) return 0.0;
        det *= U(k, k);
        for (int i = k + 1; i < n; ++i) {
            double f = U(i, k) / U(k, k);
            for (int j = k; j < n; ++j) U(i, j) -= f * U(k, j);
        }
    }
    return (swaps % 2 == 0) ? det : -det;
}

Matrix Matrix::inverse() const {
    if (!isSquare()) throw std::invalid_argument("Inverse: requires square matrix.");
    int n = rows_;
    Matrix Aug(n, 2 * n, 0.0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) Aug(i, j) = at(i, j);
        Aug(i, n + i) = 1.0;
    }
    // Gauss-Jordan on augmented [A | I]
    for (int k = 0; k < n; ++k) {
        int pivot = k;
        for (int i = k + 1; i < n; ++i)
            if (std::abs(Aug(i, k)) > std::abs(Aug(pivot, k))) pivot = i;
        Aug.swapRows(k, pivot);
        double diag = Aug(k, k);
        if (std::abs(diag) < EPS) throw std::runtime_error("Matrix is singular — cannot invert.");
        for (int j = 0; j < 2 * n; ++j) Aug(k, j) /= diag;
        for (int i = 0; i < n; ++i) {
            if (i == k) continue;
            double f = Aug(i, k);
            for (int j = 0; j < 2 * n; ++j) Aug(i, j) -= f * Aug(k, j);
        }
    }
    return Aug.submatrix(0, n, n, 2 * n);
}

int Matrix::rank(double tol) const {
    Matrix U = *this;
    int r = 0;
    for (int k = 0; k < std::min(rows_, cols_); ++k) {
        int pivot = -1;
        for (int i = k; i < rows_; ++i)
            if (std::abs(U(i, k)) > tol) { pivot = i; break; }
        if (pivot == -1) continue;
        ++r;
        U.swapRows(k, pivot);
        for (int i = k + 1; i < rows_; ++i) {
            double f = U(i, k) / U(k, k);
            for (int j = k; j < cols_; ++j) U(i, j) -= f * U(k, j);
        }
    }
    return r;
}

int Matrix::nullity() const { return cols_ - rank(); }

Matrix Matrix::pseudoInverse(double tol) const {
    auto [U, S, Vt] = svd();
    int m = rows_, n = cols_, k = std::min(m, n);
    Matrix Sp(n, m, 0.0);
    for (int i = 0; i < k; ++i)
        if (S(i, i) > tol) Sp(i, i) = 1.0 / S(i, i);
    return Vt.transpose() * Sp * U.transpose();
}

double Matrix::conditionNumber() const {
    auto [U, S, Vt] = svd();
    int k = std::min(rows_, cols_);
    double smax = S(0, 0), smin = S(k - 1, k - 1);
    if (std::abs(smin) < EPS) return std::numeric_limits<double>::infinity();
    return smax / smin;
}

// ══════════════════════════════════════════════════════════════════════════════
//  Display
// ══════════════════════════════════════════════════════════════════════════════

void Matrix::print(int precision, const std::string& label) const {
    if (!label.empty()) std::cout << label << "\n";
    std::cout << std::fixed << std::setprecision(precision);
    for (int i = 0; i < rows_; ++i) {
        std::cout << "[ ";
        for (int j = 0; j < cols_; ++j) {
            std::cout << std::setw(precision + 6) << at(i, j);
            if (j < cols_ - 1) std::cout << "  ";
        }
        std::cout << " ]\n";
    }
}

std::ostream& operator<<(std::ostream& os, const Matrix& M) {
    M.print(4, "");
    return os;
}

} // namespace Numerical