#pragma once
#include <vector>
#include <functional>
#include <string>
#include <stdexcept>
#include <iostream>
#include <tuple>
#include <optional>

namespace Numerical {

/**
 * @class Matrix
 * @brief Dense matrix class with a comprehensive suite of numerical linear
 *        algebra methods.
 *
 * All indices are 0-based.  The class stores data in row-major order.
 *
 * Usage:
 * @code
 *   Matrix A(3, 3);
 *   A(0,0)=2; A(0,1)=1; ...
 *   auto [L, U, P] = A.luDecomposition();
 *   Matrix x = A.solve(b);
 * @endcode
 */
class Matrix {
public:
    // ── Constructors ──────────────────────────────────────────────────────────

    Matrix();                                  ///< Empty (0×0)
    Matrix(int rows, int cols, double init=0); ///< m×n filled with init
    Matrix(int n, double init = 0);            ///< n×n square

    /** Construct from nested initializer list  {{1,2},{3,4}} */
    Matrix(std::initializer_list<std::initializer_list<double>> data);

    /** Construct from 2-D std::vector */
    explicit Matrix(const std::vector<std::vector<double>>& data);

    // ── Factory methods ───────────────────────────────────────────────────────

    static Matrix identity(int n);                    ///< Iₙ
    static Matrix zeros(int rows, int cols);
    static Matrix ones(int rows, int cols);
    static Matrix diagonal(const std::vector<double>& diag);
    static Matrix random(int rows, int cols, double lo=0, double hi=1);
    static Matrix hilbert(int n);                     ///< Hilbert matrix
    static Matrix companion(const std::vector<double>& coeffs); ///< Companion matrix of a polynomial

    // ── Element access ────────────────────────────────────────────────────────

    double&       operator()(int r, int c);
    const double& operator()(int r, int c) const;

    int rows() const;
    int cols() const;
    bool isSquare() const;

    // ── Row / column / submatrix access ──────────────────────────────────────

    std::vector<double> getRow(int r) const;
    std::vector<double> getCol(int c) const;
    void setRow(int r, const std::vector<double>& v);
    void setCol(int c, const std::vector<double>& v);

    /** Extract sub-matrix rows [r0,r1) × cols [c0,c1) */
    Matrix submatrix(int r0, int r1, int c0, int c1) const;

    // ── Arithmetic ────────────────────────────────────────────────────────────

    Matrix operator+(const Matrix& rhs) const;
    Matrix operator-(const Matrix& rhs) const;
    Matrix operator*(const Matrix& rhs) const;
    Matrix operator*(double s) const;
    Matrix operator/(double s) const;
    Matrix operator-()         const;

    Matrix& operator+=(const Matrix& rhs);
    Matrix& operator-=(const Matrix& rhs);
    Matrix& operator*=(double s);

    bool operator==(const Matrix& rhs) const;

    friend Matrix operator*(double s, const Matrix& M);

    /** Element-wise (Hadamard) product */
    Matrix hadamard(const Matrix& rhs) const;

    // ── Basic operations ──────────────────────────────────────────────────────

    Matrix  transpose() const;
    double  trace()     const;
    double  frobenius() const;   ///< Frobenius norm √(Σ aᵢⱼ²)
    double  norm1()     const;   ///< Max column sum
    double  normInf()   const;   ///< Max row sum

    // ── Matrix properties ─────────────────────────────────────────────────────

    bool isSymmetric(double tol = 1e-9)     const;
    bool isOrthogonal(double tol = 1e-9)    const;
    bool isDiagonallyDominant()             const;
    bool isPositiveDefinite()               const;
    bool isSingular(double tol = 1e-12)     const;

    // ── Decompositions ────────────────────────────────────────────────────────

    /**
     * @brief LU Decomposition with partial pivoting (Doolittle form).
     * @return {L, U, P} such that P·A = L·U.
     *         L is unit-lower-triangular, U is upper-triangular.
     */
    std::tuple<Matrix, Matrix, Matrix> luDecomposition() const;

    /**
     * @brief LDLᵀ Decomposition (for symmetric positive-definite matrices).
     * @return {L, D} such that A = L·D·Lᵀ.
     */
    std::pair<Matrix, Matrix> ldltDecomposition() const;

    /**
     * @brief Cholesky Decomposition (A = L·Lᵀ) for SPD matrices.
     * @return Lower-triangular L.
     */
    Matrix choleskyDecomposition() const;

    /**
     * @brief QR Decomposition via Householder reflections.
     * @return {Q, R} such that A = Q·R, Q orthogonal, R upper-triangular.
     */
    std::pair<Matrix, Matrix> qrDecomposition() const;

    /**
     * @brief Singular Value Decomposition (A = U·Σ·Vᵀ) via Golub-Reinsch.
     * @return {U, Sigma, V} where Sigma is diagonal and singular values are
     *         sorted descending.
     */
    std::tuple<Matrix, Matrix, Matrix> svd() const;

    /**
     * @brief Hessenberg Reduction via Householder (for non-symmetric matrices).
     * @return {Q, H} such that A = Q·H·Qᵀ.
     */
    std::pair<Matrix, Matrix> hessenbergReduction() const;

    /**
     * @brief Schur Decomposition via QR iteration.
     * @return {Q, T} such that A = Q·T·Qᵀ (T quasi-upper-triangular).
     */
    std::pair<Matrix, Matrix> schurDecomposition() const;

    // ── Solve linear systems ──────────────────────────────────────────────────

    /**
     * @brief Solve A·x = b using LU with partial pivoting.
     * @param b  Right-hand side (column vector as Matrix nx1 or vector).
     */
    Matrix solve(const Matrix& b) const;

    /**
     * @brief Solve using Cholesky (SPD matrices only, faster).
     */
    Matrix solveSPD(const Matrix& b) const;

    /**
     * @brief Least-squares solution via QR: min‖Ax−b‖.
     */
    Matrix solveLeastSquares(const Matrix& b) const;

    /**
     * @brief Gaussian Elimination with partial pivoting.
     * @return Solution x of A·x = b.
     */
    Matrix gaussianElimination(const Matrix& b) const;

    /**
     * @brief Gauss-Jordan Elimination — produces reduced row echelon form.
     */
    Matrix gaussJordan(const Matrix& b) const;

    /**
     * @brief Thomas Algorithm (tridiagonal solver) — O(n).
     * @param lower  Sub-diagonal (size n-1)
     * @param main   Main diagonal  (size n)
     * @param upper  Super-diagonal (size n-1)
     * @param rhs    Right-hand side (size n)
     */
    static std::vector<double> thomasAlgorithm(
        const std::vector<double>& lower,
        const std::vector<double>& main,
        const std::vector<double>& upper,
        const std::vector<double>& rhs);

    // ── Iterative solvers ─────────────────────────────────────────────────────

    /**
     * @brief Jacobi Iteration for A·x = b.
     * @param b     RHS
     * @param x0    Initial guess (default zero)
     * @param tol   Tolerance
     * @param maxIt Max iterations
     */
    Matrix jacobiIteration(const Matrix& b,
                            Matrix x0 = Matrix(),
                            double tol = 1e-10,
                            int maxIt  = 1000) const;

    /**
     * @brief Gauss-Seidel Iteration for A·x = b.
     */
    Matrix gaussSeidelIteration(const Matrix& b,
                                 Matrix x0 = Matrix(),
                                 double tol = 1e-10,
                                 int maxIt  = 1000) const;

    /**
     * @brief Successive Over-Relaxation (SOR).
     * @param omega  Relaxation factor (1 = Gauss-Seidel, 0 < ω < 2)
     */
    Matrix sor(const Matrix& b,
               double omega,
               Matrix x0 = Matrix(),
               double tol = 1e-10,
               int maxIt  = 1000) const;

    /**
     * @brief Conjugate Gradient Method (for SPD matrices).
     */
    Matrix conjugateGradient(const Matrix& b,
                              Matrix x0 = Matrix(),
                              double tol = 1e-10,
                              int maxIt  = 1000) const;

    // ── Eigenvalues & vectors ──────────────────────────────────────────────────

    /**
     * @brief Power Iteration — dominant eigenvalue/vector.
     * @return {eigenvalue, eigenvector}
     */
    std::pair<double, Matrix> powerIteration(double tol = 1e-10,
                                              int maxIt  = 1000) const;

    /**
     * @brief Inverse Iteration — eigenvalue closest to a shift μ.
     */
    std::pair<double, Matrix> inverseIteration(double mu,
                                                double tol = 1e-10,
                                                int maxIt  = 1000) const;

    /**
     * @brief QR Algorithm — all eigenvalues of a symmetric matrix.
     * @return Vector of real eigenvalues (sorted descending).
     */
    std::vector<double> eigenvaluesQR(double tol = 1e-10, int maxIt = 1000) const;

    /**
     * @brief Jacobi Eigenvalue Method (symmetric matrices only).
     * @return {eigenvalues, eigenvectors}  (columns of eigenvectors)
     */
    std::pair<std::vector<double>, Matrix> jacobiEigen(double tol = 1e-10,
                                                         int maxIt  = 100) const;

    // ── Miscellaneous ─────────────────────────────────────────────────────────

    double determinant() const;
    Matrix inverse()     const;
    int    rank(double tol = 1e-9) const;
    int    nullity()     const;

    /** Pseudoinverse (Moore-Penrose) via SVD */
    Matrix pseudoInverse(double tol = 1e-9) const;

    /** Compute condition number (ratio of largest to smallest singular value) */
    double conditionNumber() const;

    // ── Display ───────────────────────────────────────────────────────────────

    void print(int precision = 4,
               const std::string& label = "") const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix& M);

private:
    int rows_, cols_;
    std::vector<double> data_;   ///< Row-major flat storage

    double& at(int r, int c);
    const double& at(int r, int c) const;

    // Internal helpers
    Matrix  forwardSubstitution (const Matrix& L, const Matrix& b) const;
    Matrix  backSubstitution    (const Matrix& U, const Matrix& b) const;
    Matrix  applyPermutation    (const Matrix& P, const Matrix& b) const;
    void    swapRows            (int r1, int r2);
    int     pivotRow            (int col, int startRow) const;
    double  householderNorm     (int col, int startRow) const;

    static constexpr double EPS = 1e-12;
};

} // namespace Numerical