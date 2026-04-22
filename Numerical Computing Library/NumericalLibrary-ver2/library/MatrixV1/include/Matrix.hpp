#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

// Method to use when computing the determinant
enum class DetMethod { LU, Cofactor };

class Matrix {
private:
    int rows, cols;
    std::vector<std::vector<double>> data;

    // Internal helper for cofactor expansion (recursive)
    double cofactorDet() const;

    // Returns the submatrix with row r and col c removed
    Matrix submatrix(int r, int c) const;

public:
    // ─── Construction ────────────────────────────────────────────────────────
    Matrix(int r = 0, int c = 0);

    // ─── I/O (original interface — unchanged) ────────────────────────────────
    void input();
    void inputFromFile(const std::string& filename);
    void display() const;
    void saveToFile(const std::string& filename) const;

    // ─── Accessors ───────────────────────────────────────────────────────────
    int getRows() const;
    int getCols() const;

    // Bounds-checked: throw std::out_of_range if indices are invalid
    double get(int i, int j) const;
    void   set(int i, int j, double val);

    // ─── Predicates ──────────────────────────────────────────────────────────
    bool isSquare()    const;
    bool isSymmetric() const;   // tolerance 1e-9; returns false if not square

    // ─── Utility methods ─────────────────────────────────────────────────────
    Matrix transpose()   const;
    double determinant(DetMethod method = DetMethod::LU) const;
    Matrix inverse()     const;  // Gauss-Jordan; throws if singular or non-square

    // ─── Arithmetic operators (delegate to static operation classes) ─────────
    friend Matrix operator+(const Matrix& A, const Matrix& B);
    friend Matrix operator-(const Matrix& A, const Matrix& B);
    friend Matrix operator*(const Matrix& A, const Matrix& B);

    // ─── Comparison operators (element-wise, tolerance 1e-9) ─────────────────
    friend bool operator==(const Matrix& A, const Matrix& B);
    friend bool operator!=(const Matrix& A, const Matrix& B);

    // ─── Stream output ───────────────────────────────────────────────────────
    friend std::ostream& operator<<(std::ostream& os, const Matrix& M);
};

#endif
