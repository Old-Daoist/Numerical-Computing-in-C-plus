#include "../include/Matrix.hpp"
#include "../include/operations/Add.hpp"
#include "../include/operations/Subtract.hpp"
#include "../include/operations/Multiply.hpp"
#include <fstream>
#include <cmath>
#include <sstream>

// ─── Construction ─────────────────────────────────────────────────────────────

Matrix::Matrix(int r, int c) : rows(r), cols(c) {
    data.resize(r, std::vector<double>(c, 0));
}

// ─── I/O ──────────────────────────────────────────────────────────────────────

void Matrix::input() {
    std::cout << "Enter elements row-wise:\n";
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            std::cin >> data[i][j];
}

void Matrix::inputFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file: " + filename);

    file >> rows >> cols;
    data.assign(rows, std::vector<double>(cols));

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            file >> data[i][j];
}

void Matrix::display() const {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            std::cout << data[i][j] << " ";
        std::cout << "\n";
    }
}

void Matrix::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file for writing: " + filename);

    file << rows << " " << cols << "\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            file << data[i][j] << " ";
        file << "\n";
    }
}

// ─── Accessors ────────────────────────────────────────────────────────────────

int Matrix::getRows() const { return rows; }
int Matrix::getCols() const { return cols; }

double Matrix::get(int i, int j) const {
    if (i < 0 || i >= rows || j < 0 || j >= cols) {
        std::ostringstream oss;
        oss << "Matrix::get(" << i << ", " << j << ") out of range ["
            << rows << "x" << cols << "]";
        throw std::out_of_range(oss.str());
    }
    return data[i][j];
}

void Matrix::set(int i, int j, double val) {
    if (i < 0 || i >= rows || j < 0 || j >= cols) {
        std::ostringstream oss;
        oss << "Matrix::set(" << i << ", " << j << ") out of range ["
            << rows << "x" << cols << "]";
        throw std::out_of_range(oss.str());
    }
    data[i][j] = val;
}

// ─── Predicates ───────────────────────────────────────────────────────────────

bool Matrix::isSquare() const {
    return rows == cols;
}

bool Matrix::isSymmetric() const {
    if (!isSquare()) return false;
    const double tol = 1e-9;
    for (int i = 0; i < rows; i++)
        for (int j = i + 1; j < cols; j++)
            if (std::abs(data[i][j] - data[j][i]) > tol)
                return false;
    return true;
}

// ─── Utility methods ──────────────────────────────────────────────────────────

Matrix Matrix::transpose() const {
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.data[j][i] = data[i][j];
    return result;
}

// --- Submatrix helper (for cofactor expansion) ---
Matrix Matrix::submatrix(int skipRow, int skipCol) const {
    Matrix result(rows - 1, cols - 1);
    int ri = 0;
    for (int i = 0; i < rows; i++) {
        if (i == skipRow) continue;
        int rj = 0;
        for (int j = 0; j < cols; j++) {
            if (j == skipCol) continue;
            result.data[ri][rj++] = data[i][j];
        }
        ri++;
    }
    return result;
}

// --- Cofactor expansion (recursive) ---
double Matrix::cofactorDet() const {
    if (rows == 1) return data[0][0];
    if (rows == 2) return data[0][0] * data[1][1] - data[0][1] * data[1][0];

    double det = 0.0;
    for (int j = 0; j < cols; j++) {
        double sign = (j % 2 == 0) ? 1.0 : -1.0;
        det += sign * data[0][j] * submatrix(0, j).cofactorDet();
    }
    return det;
}

// --- determinant() ---
double Matrix::determinant(DetMethod method) const {
    if (!isSquare())
        throw std::runtime_error("determinant() requires a square matrix");

    if (method == DetMethod::Cofactor)
        return cofactorDet();

    // LU-based: partial pivoting, track swap count for sign
    int n = rows;
    Matrix A(*this);
    int swaps = 0;

    for (int i = 0; i < n; i++) {
        // Find pivot
        int pivot = i;
        for (int p = i + 1; p < n; p++)
            if (std::abs(A.data[p][i]) > std::abs(A.data[pivot][i]))
                pivot = p;

        if (std::abs(A.data[pivot][i]) < 1e-12)
            return 0.0;  // singular — determinant is zero

        if (pivot != i) {
            std::swap(A.data[i], A.data[pivot]);
            swaps++;
        }

        for (int j = i + 1; j < n; j++) {
            double factor = A.data[j][i] / A.data[i][i];
            for (int k = i; k < n; k++)
                A.data[j][k] -= factor * A.data[i][k];
        }
    }

    double det = (swaps % 2 == 0) ? 1.0 : -1.0;
    for (int i = 0; i < n; i++)
        det *= A.data[i][i];
    return det;
}

// --- inverse() — Gauss-Jordan elimination on [A | I] ---
Matrix Matrix::inverse() const {
    if (!isSquare())
        throw std::runtime_error("inverse() requires a square matrix");

    int n = rows;

    // Build augmented matrix [A | I]
    Matrix aug(n, 2 * n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            aug.data[i][j] = data[i][j];
        aug.data[i][n + i] = 1.0;  // identity block
    }

    // Forward elimination with partial pivoting
    for (int i = 0; i < n; i++) {
        // Find pivot
        int pivot = i;
        for (int p = i + 1; p < n; p++)
            if (std::abs(aug.data[p][i]) > std::abs(aug.data[pivot][i]))
                pivot = p;

        if (std::abs(aug.data[pivot][i]) < 1e-12)
            throw std::runtime_error("Matrix is singular — inverse does not exist");

        if (pivot != i)
            std::swap(aug.data[i], aug.data[pivot]);

        // Scale pivot row
        double scale = aug.data[i][i];
        for (int j = 0; j < 2 * n; j++)
            aug.data[i][j] /= scale;

        // Eliminate column above and below
        for (int k = 0; k < n; k++) {
            if (k == i) continue;
            double factor = aug.data[k][i];
            for (int j = 0; j < 2 * n; j++)
                aug.data[k][j] -= factor * aug.data[i][j];
        }
    }

    // Extract right block as the inverse
    Matrix result(n, n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            result.data[i][j] = aug.data[i][n + j];

    return result;
}

// ─── Arithmetic operators ─────────────────────────────────────────────────────

Matrix operator+(const Matrix& A, const Matrix& B) { return Add::compute(A, B); }
Matrix operator-(const Matrix& A, const Matrix& B) { return Subtract::compute(A, B); }
Matrix operator*(const Matrix& A, const Matrix& B) { return Multiply::compute(A, B); }

// ─── Comparison operators ─────────────────────────────────────────────────────

bool operator==(const Matrix& A, const Matrix& B) {
    if (A.rows != B.rows || A.cols != B.cols) return false;
    const double tol = 1e-9;
    for (int i = 0; i < A.rows; i++)
        for (int j = 0; j < A.cols; j++)
            if (std::abs(A.data[i][j] - B.data[i][j]) > tol)
                return false;
    return true;
}

bool operator!=(const Matrix& A, const Matrix& B) { return !(A == B); }

// ─── Stream output ────────────────────────────────────────────────────────────

std::ostream& operator<<(std::ostream& os, const Matrix& M) {
    for (int i = 0; i < M.rows; i++) {
        for (int j = 0; j < M.cols; j++) {
            os << M.data[i][j];
            if (j < M.cols - 1) os << " ";
        }
        os << "\n";
    }
    return os;
}
