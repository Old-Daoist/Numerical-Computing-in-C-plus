#include "catch2/catch_amalgamated.hpp"
#include "Matrix.hpp"
#include "../include/operations/Divide.hpp"

// ─── Helper: build a matrix from an initializer list ─────────────────────────
static Matrix make(int r, int c, std::vector<double> vals) {
    Matrix m(r, c);
    int k = 0;
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            m.set(i, j, vals[k++]);
    return m;
}

// ─── isSquare ─────────────────────────────────────────────────────────────────

TEST_CASE("isSquare returns true for square matrices", "[predicates]") {
    REQUIRE(Matrix(3, 3).isSquare() == true);
    REQUIRE(Matrix(1, 1).isSquare() == true);
}

TEST_CASE("isSquare returns false for non-square matrices", "[predicates]") {
    REQUIRE(Matrix(2, 3).isSquare() == false);
    REQUIRE(Matrix(3, 2).isSquare() == false);
}

// ─── isSymmetric ──────────────────────────────────────────────────────────────

TEST_CASE("isSymmetric returns true for symmetric matrix", "[predicates]") {
    Matrix m = make(3, 3, {
        1, 2, 3,
        2, 5, 6,
        3, 6, 9
    });
    REQUIRE(m.isSymmetric() == true);
}

TEST_CASE("isSymmetric returns false for non-symmetric matrix", "[predicates]") {
    Matrix m = make(3, 3, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    });
    REQUIRE(m.isSymmetric() == false);
}

TEST_CASE("isSymmetric returns false for non-square matrix", "[predicates]") {
    REQUIRE(Matrix(2, 3).isSymmetric() == false);
}

TEST_CASE("identity matrix is symmetric", "[predicates]") {
    Matrix eye(3, 3);
    eye.set(0,0,1); eye.set(1,1,1); eye.set(2,2,1);
    REQUIRE(eye.isSymmetric() == true);
}

// ─── Bounds checking ──────────────────────────────────────────────────────────

TEST_CASE("get() throws out_of_range for negative row", "[bounds]") {
    Matrix m(3, 3);
    REQUIRE_THROWS(m.get(-1, 0));
}

TEST_CASE("get() throws out_of_range for row >= rows", "[bounds]") {
    Matrix m(3, 3);
    REQUIRE_THROWS(m.get(3, 0));
}

TEST_CASE("get() throws out_of_range for negative col", "[bounds]") {
    Matrix m(3, 3);
    REQUIRE_THROWS(m.get(0, -1));
}

TEST_CASE("get() throws out_of_range for col >= cols", "[bounds]") {
    Matrix m(3, 3);
    REQUIRE_THROWS(m.get(0, 3));
}

TEST_CASE("set() throws out_of_range for invalid index", "[bounds]") {
    Matrix m(2, 2);
    REQUIRE_THROWS(m.set(5, 0, 1.0));
}

TEST_CASE("get() and set() work correctly at boundary indices", "[bounds]") {
    Matrix m(3, 4);
    REQUIRE_NOTHROW(m.set(2, 3, 7.7));
    REQUIRE(m.get(2, 3) == 7.7);
}

// ─── transpose ────────────────────────────────────────────────────────────────

TEST_CASE("transpose of 2x3 gives 3x2", "[transpose]") {
    Matrix m = make(2, 3, {1, 2, 3, 4, 5, 6});
    Matrix t = m.transpose();
    REQUIRE(t.getRows() == 3);
    REQUIRE(t.getCols() == 2);
}

TEST_CASE("transpose swaps elements correctly", "[transpose]") {
    Matrix m = make(2, 3, {1, 2, 3, 4, 5, 6});
    Matrix t = m.transpose();
    REQUIRE(t.get(0, 0) == 1.0);
    REQUIRE(t.get(0, 1) == 4.0);
    REQUIRE(t.get(1, 0) == 2.0);
    REQUIRE(t.get(2, 1) == 6.0);
}

TEST_CASE("transpose of square matrix works", "[transpose]") {
    Matrix m = make(2, 2, {1, 2, 3, 4});
    Matrix t = m.transpose();
    REQUIRE(t.get(0, 1) == 3.0);
    REQUIRE(t.get(1, 0) == 2.0);
}

TEST_CASE("double transpose equals original", "[transpose]") {
    Matrix m = make(2, 3, {1, 2, 3, 4, 5, 6});
    REQUIRE(m.transpose().transpose() == m);
}

// ─── determinant ──────────────────────────────────────────────────────────────

TEST_CASE("determinant of 1x1 matrix", "[determinant]") {
    Matrix m = make(1, 1, {7.0});
    REQUIRE(std::abs(m.determinant() - 7.0) < 1e-9);
}

TEST_CASE("determinant of 2x2 matrix (LU)", "[determinant]") {
    Matrix m = make(2, 2, {3, 8, 4, 6});
    // det = 3*6 - 8*4 = 18 - 32 = -14
    REQUIRE(std::abs(m.determinant(DetMethod::LU) - (-14.0)) < 1e-9);
}

TEST_CASE("determinant of 2x2 matrix (Cofactor)", "[determinant]") {
    Matrix m = make(2, 2, {3, 8, 4, 6});
    REQUIRE(std::abs(m.determinant(DetMethod::Cofactor) - (-14.0)) < 1e-9);
}

TEST_CASE("determinant of 3x3 matrix — both methods agree", "[determinant]") {
    Matrix m = make(3, 3, {
        6, 1, 1,
        4, -2, 5,
        2, 8, 7
    });
    double lu  = m.determinant(DetMethod::LU);
    double cof = m.determinant(DetMethod::Cofactor);
    REQUIRE(std::abs(lu - cof) < 1e-9);
    // det = 6(-14-40) - 1(28-10) + 1(32+4) = 6(-54) - 18 + 36 = -324 - 18 + 36 = -306
    REQUIRE(std::abs(lu - (-306.0)) < 1e-9);
}

TEST_CASE("determinant of singular matrix is zero", "[determinant]") {
    Matrix m = make(3, 3, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    });
    REQUIRE(std::abs(m.determinant()) < 1e-9);
}

TEST_CASE("determinant throws on non-square matrix", "[determinant]") {
    Matrix m(2, 3);
    REQUIRE_THROWS(m.determinant());
}

TEST_CASE("identity matrix determinant is 1", "[determinant]") {
    Matrix eye(3, 3);
    eye.set(0,0,1); eye.set(1,1,1); eye.set(2,2,1);
    REQUIRE(std::abs(eye.determinant() - 1.0) < 1e-9);
}

// ─── inverse ──────────────────────────────────────────────────────────────────

TEST_CASE("inverse of 2x2 matrix is correct", "[inverse]") {
    Matrix m = make(2, 2, {4, 7, 2, 6});
    Matrix inv = m.inverse();
    // Expected: [0.6, -0.7; -0.2, 0.4]
    REQUIRE(std::abs(inv.get(0,0) -  0.6) < 1e-9);
    REQUIRE(std::abs(inv.get(0,1) - (-0.7)) < 1e-9);
    REQUIRE(std::abs(inv.get(1,0) - (-0.2)) < 1e-9);
    REQUIRE(std::abs(inv.get(1,1) -  0.4) < 1e-9);
}

TEST_CASE("A * inverse(A) equals identity", "[inverse]") {
    Matrix m = make(3, 3, {
        2, 1, 1,
        3, 2, 1,
        2, 1, 2
    });
    Matrix inv = m.inverse();
    Matrix product = m * inv;
    // Should be identity within tolerance
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            REQUIRE(std::abs(product.get(i,j) - expected) < 1e-9);
        }
}

TEST_CASE("inverse of identity is identity", "[inverse]") {
    Matrix eye(3, 3);
    eye.set(0,0,1); eye.set(1,1,1); eye.set(2,2,1);
    Matrix inv = eye.inverse();
    REQUIRE(inv == eye);
}

TEST_CASE("inverse throws on singular matrix", "[inverse]") {
    Matrix m = make(3, 3, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    });
    REQUIRE_THROWS(m.inverse());
}

TEST_CASE("inverse throws on non-square matrix", "[inverse]") {
    Matrix m(2, 3);
    REQUIRE_THROWS(m.inverse());
}

// ─── operator+ ────────────────────────────────────────────────────────────────

TEST_CASE("operator+ produces correct result", "[operators]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {5, 6, 7, 8});
    Matrix C = A + B;
    REQUIRE(C.get(0,0) == 6.0);
    REQUIRE(C.get(1,1) == 12.0);
}

TEST_CASE("operator+ throws on size mismatch", "[operators]") {
    Matrix A(2, 2), B(3, 3);
    REQUIRE_THROWS(A + B);
}

// ─── operator- ────────────────────────────────────────────────────────────────

TEST_CASE("operator- produces correct result", "[operators]") {
    Matrix A = make(2, 2, {5, 6, 7, 8});
    Matrix B = make(2, 2, {1, 2, 3, 4});
    Matrix C = A - B;
    REQUIRE(C.get(0,0) == 4.0);
    REQUIRE(C.get(1,1) == 4.0);
}

// ─── operator* ────────────────────────────────────────────────────────────────

TEST_CASE("operator* produces correct result", "[operators]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {5, 6, 7, 8});
    Matrix C = A * B;
    // [1*5+2*7, 1*6+2*8; 3*5+4*7, 3*6+4*8] = [19,22; 43,50]
    REQUIRE(C.get(0,0) == 19.0);
    REQUIRE(C.get(0,1) == 22.0);
    REQUIRE(C.get(1,0) == 43.0);
    REQUIRE(C.get(1,1) == 50.0);
}

TEST_CASE("operator* throws on incompatible dimensions", "[operators]") {
    Matrix A(2, 3), B(2, 2);
    REQUIRE_THROWS(A * B);
}

// ─── operator== / operator!= ─────────────────────────────────────────────────

TEST_CASE("operator== returns true for equal matrices", "[operators]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {1, 2, 3, 4});
    REQUIRE(A == B);
}

TEST_CASE("operator== returns false for different values", "[operators]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {1, 2, 3, 5});
    REQUIRE_FALSE(A == B);
}

TEST_CASE("operator== returns false for different sizes", "[operators]") {
    Matrix A(2, 2), B(3, 3);
    REQUIRE_FALSE(A == B);
}

TEST_CASE("operator!= works correctly", "[operators]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {1, 2, 3, 5});
    REQUIRE(A != B);
}

// ─── operator<< ───────────────────────────────────────────────────────────────

TEST_CASE("operator<< writes matrix to stream", "[operators]") {
    Matrix m = make(2, 2, {1, 2, 3, 4});
    std::ostringstream oss;
    oss << m;
    std::string out = oss.str();
    REQUIRE(out.find("1") != std::string::npos);
    REQUIRE(out.find("4") != std::string::npos);
}

// ─── Divide (N×N) ─────────────────────────────────────────────────────────────

TEST_CASE("Divide 2x2 matrices", "[divide]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {2, 0, 0, 1});
    Matrix result = Divide::compute(A, B);
    // inv(B) = [[0.5, 0], [0, 1]]
    // A * inv(B) = [[1,2],[3,4]] * [[0.5,0],[0,1]] = [[0.5, 2], [1.5, 4]]
    Matrix expected = make(2, 2, {0.5, 2, 1.5, 4});
    REQUIRE(result == expected);
}

TEST_CASE("Divide 3x3 matrices", "[divide]") {
    Matrix A = make(2, 3, {1, 2, 3, 4, 5, 6});  // 2x3
    Matrix B = make(3, 3, {1, 0, 0, 0, 1, 0, 0, 0, 1});  // identity
    Matrix result = Divide::compute(A, B);
    // inv(B) = identity, so result = A
    REQUIRE(result == A);
}

TEST_CASE("Divide throws on dimension mismatch", "[divide]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});  // 2x2
    Matrix B = make(3, 3, {1, 0, 0, 0, 1, 0, 0, 0, 1});  // 3x3
    REQUIRE_THROWS_AS(Divide::compute(A, B), std::invalid_argument);
}

TEST_CASE("Divide throws on singular matrix", "[divide]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 2, {1, 2, 2, 4});  // singular (rank 1)
    REQUIRE_THROWS_AS(Divide::compute(A, B), std::runtime_error);
}

TEST_CASE("Divide throws on non-square matrix", "[divide]") {
    Matrix A = make(2, 2, {1, 2, 3, 4});
    Matrix B = make(2, 3, {1, 2, 3, 4, 5, 6});  // 2x3
    REQUIRE_THROWS_AS(Divide::compute(A, B), std::runtime_error);
}
