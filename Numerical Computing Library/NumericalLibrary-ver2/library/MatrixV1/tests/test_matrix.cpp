#include "catch2/catch_amalgamated.hpp"
#include "Matrix.hpp"
#include "../include/operations/Divide.hpp"
#include "../include/solvers/SolveLinearEquation.hpp"
#include "../include/solvers/Cholesky.hpp"
#include "../include/solvers/QR.hpp"
#include "../include/numerical/Interpolation.hpp"
#include "../include/numerical/EigenPower.hpp"

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

// ─── Gauss-Jacobi Convergence ─────────────────────────────────────────────────

TEST_CASE("Jacobi converges within tolerance", "[solvers]") {
    // Diagonally dominant system: convergence guaranteed
    Matrix A = make(3, 3, {
        10, 1, 1,
        1, 10, 1,
        1, 1, 10
    });
    std::vector<double> b = {12, 12, 12};
    
    int iters = 0;
    std::vector<double> x = GaussJacobi::solve(A, b, 1e-6, 1000, iters);
    
    // Solution should be approximately [1, 1, 1]
    REQUIRE(std::abs(x[0] - 1.0) < 1e-5);
    REQUIRE(std::abs(x[1] - 1.0) < 1e-5);
    REQUIRE(std::abs(x[2] - 1.0) < 1e-5);
    REQUIRE(iters < 1000);  // Should converge before max iterations
}

TEST_CASE("Jacobi iteration count reflects convergence speed", "[solvers]") {
    Matrix A = make(3, 3, {
        10, 1, 1,
        1, 10, 1,
        1, 1, 10
    });
    std::vector<double> b = {12, 12, 12};
    
    int iters_tight = 0, iters_loose = 0;
    
    // Tight tolerance should need more iterations
    GaussJacobi::solve(A, b, 1e-10, 1000, iters_tight);
    
    // Loose tolerance should need fewer iterations
    GaussJacobi::solve(A, b, 1e-3, 1000, iters_loose);
    
    REQUIRE(iters_tight > iters_loose);
}

TEST_CASE("Jacobi hits max iterations if system difficult", "[solvers]") {
    // Nearly singular system (hard to converge)
    Matrix A = make(2, 2, {
        1, 1-1e-10,
        1-1e-10, 1
    });
    std::vector<double> b = {2, 2};
    
    int iters = 0;
    GaussJacobi::solve(A, b, 1e-12, 10, iters);
    
    REQUIRE(iters == 10);  // Should hit the max
}

// ─── Gauss-Seidel Convergence ────────────────────────────────────────────────

TEST_CASE("Seidel converges within tolerance", "[solvers]") {
    // Diagonally dominant system: convergence guaranteed
    Matrix A = make(3, 3, {
        10, 1, 1,
        1, 10, 1,
        1, 1, 10
    });
    std::vector<double> b = {12, 12, 12};
    
    int iters = 0;
    std::vector<double> x = GaussSeidel::solve(A, b, 1e-6, 1000, iters);
    
    // Solution should be approximately [1, 1, 1]
    REQUIRE(std::abs(x[0] - 1.0) < 1e-5);
    REQUIRE(std::abs(x[1] - 1.0) < 1e-5);
    REQUIRE(std::abs(x[2] - 1.0) < 1e-5);
    REQUIRE(iters < 1000);  // Should converge before max iterations
}

TEST_CASE("Seidel typically converges faster than Jacobi", "[solvers]") {
    // Diagonally dominant system
    Matrix A = make(3, 3, {
        10, 1, 1,
        1, 10, 1,
        1, 1, 10
    });
    std::vector<double> b = {12, 12, 12};
    
    int jacobi_iters = 0, seidel_iters = 0;
    
    GaussJacobi::solve(A, b, 1e-6, 1000, jacobi_iters);
    GaussSeidel::solve(A, b, 1e-6, 1000, seidel_iters);
    
    // Seidel should typically converge faster (may sometimes be equal)
    REQUIRE(seidel_iters <= jacobi_iters);
}

TEST_CASE("Backward compatibility: Jacobi with fixed iterations", "[solvers]") {
    Matrix A = make(3, 3, {
        10, 1, 1,
        1, 10, 1,
        1, 1, 10
    });
    std::vector<double> b = {12, 12, 12};
    
    // Old API: fixed iteration count (default 25)
    std::vector<double> x = GaussJacobi::solve(A, b);
    
    // Should still work and produce a reasonable result
    REQUIRE(std::abs(x[0] - 1.0) < 0.1);  // Loose tolerance for 25 iterations
}

TEST_CASE("Backward compatibility: Seidel with fixed iterations", "[solvers]") {
    Matrix A = make(3, 3, {
        10, 1, 1,
        1, 10, 1,
        1, 1, 10
    });
    std::vector<double> b = {12, 12, 12};
    
    // Old API: fixed iteration count (default 25)
    std::vector<double> x = GaussSeidel::solve(A, b);
    
    // Should still work and produce a reasonable result
    REQUIRE(std::abs(x[0] - 1.0) < 0.1);  // Loose tolerance for 25 iterations
}

// ─── Cholesky Decomposition ──────────────────────────────────────────────────

TEST_CASE("Cholesky decompose 2x2 SPD matrix", "[cholesky]") {
    // A = [[4, 2], [2, 3]]  →  L = [[2, 0], [1, sqrt(2)]]
    Matrix A = make(2, 2, {4, 2, 2, 3});
    Matrix L = Cholesky::decompose(A);

    REQUIRE(std::abs(L.get(0,0) - 2.0) < 1e-9);
    REQUIRE(std::abs(L.get(0,1) - 0.0) < 1e-9);
    REQUIRE(std::abs(L.get(1,0) - 1.0) < 1e-9);
    REQUIRE(std::abs(L.get(1,1) - std::sqrt(2.0)) < 1e-9);
}

TEST_CASE("Cholesky decompose 3x3: verify A = L*L^T", "[cholesky]") {
    // A = [[25, 15, -5], [15, 18, 0], [-5, 0, 11]]  (SPD)
    Matrix A = make(3, 3, {
        25, 15, -5,
        15, 18,  0,
        -5,  0, 11
    });
    Matrix L = Cholesky::decompose(A);
    Matrix LT = L.transpose();
    Matrix product = L * LT;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            REQUIRE(std::abs(product.get(i,j) - A.get(i,j)) < 1e-9);
}

TEST_CASE("Cholesky solve: known 3x3 SPD system", "[cholesky]") {
    // A = [[25, 15, -5], [15, 18, 0], [-5, 0, 11]], b = [35, 33, 6]
    // Expected solution: x = [1, 1, 1]  (25+15-5=35, 15+18+0=33, -5+0+11=6)
    Matrix A = make(3, 3, {
        25, 15, -5,
        15, 18,  0,
        -5,  0, 11
    });
    std::vector<double> b = {35, 33, 6};
    std::vector<double> x = Cholesky::solve(A, b);

    REQUIRE(std::abs(x[0] - 1.0) < 1e-9);
    REQUIRE(std::abs(x[1] - 1.0) < 1e-9);
    REQUIRE(std::abs(x[2] - 1.0) < 1e-9);
}

TEST_CASE("Cholesky throws on non-SPD matrix", "[cholesky]") {
    // Indefinite matrix: eigenvalues are not all positive
    Matrix A = make(3, 3, {
        1, 2, 3,
        2, 1, 4,
        3, 4, 1
    });
    REQUIRE_THROWS(Cholesky::decompose(A));
}

TEST_CASE("Cholesky throws on non-square matrix", "[cholesky]") {
    Matrix A(2, 3);
    REQUIRE_THROWS(Cholesky::decompose(A));
}

TEST_CASE("Cholesky decomposes identity matrix", "[cholesky]") {
    Matrix eye(3, 3);
    eye.set(0,0,1); eye.set(1,1,1); eye.set(2,2,1);
    Matrix L = Cholesky::decompose(eye);
    // L of identity should be identity
    REQUIRE(L == eye);
}

TEST_CASE("Cholesky solve 4x4 system", "[cholesky]") {
    // Diagonally dominant SPD: A[i][j] = (i==j ? n+1 : 1)
    // 4x4 with diagonal 5, off-diagonal 1:  solution = 1/(n+4) * ones
    Matrix A = make(4, 4, {
        5, 1, 1, 1,
        1, 5, 1, 1,
        1, 1, 5, 1,
        1, 1, 1, 5
    });
    std::vector<double> b = {8, 8, 8, 8};
    std::vector<double> x = Cholesky::solve(A, b);

    // Each row: 5*x + 3*x = 8x = 8 → x = 1
    for (int i = 0; i < 4; i++) {
        REQUIRE(std::abs(x[i] - 1.0) < 1e-9);
    }
}

// ─── QR Decomposition ────────────────────────────────────────────────────────

TEST_CASE("QR decompose 3x3: verify A = Q*R", "[qr]") {
    Matrix A = make(3, 3, {
        12, -51,   4,
         6, 167, -68,
        -4,  24, -41
    });
    QR::Decomposition qr = QR::decompose(A);
    Matrix product = qr.Q * qr.R;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            REQUIRE(std::abs(product.get(i,j) - A.get(i,j)) < 1e-9);
}

TEST_CASE("QR decompose: Q is orthogonal (Q^T*Q = I)", "[qr]") {
    Matrix A = make(3, 3, {
        12, -51,   4,
         6, 167, -68,
        -4,  24, -41
    });
    QR::Decomposition qr = QR::decompose(A);
    Matrix QTQ = qr.Q.transpose() * qr.Q;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            double expected = (i == j) ? 1.0 : 0.0;
            REQUIRE(std::abs(QTQ.get(i,j) - expected) < 1e-9);
        }
}

TEST_CASE("QR decompose: R is upper triangular", "[qr]") {
    Matrix A = make(3, 3, {
        1, 2, 3,
        4, 5, 6,
        7, 8, 10  // non-singular
    });
    QR::Decomposition qr = QR::decompose(A);

    // Below diagonal should be zero
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < i; j++)
            REQUIRE(std::abs(qr.R.get(i,j)) < 1e-9);
}

TEST_CASE("QR solve: square 3x3 system", "[qr]") {
    Matrix A = make(3, 3, {
        2, 1, 1,
        3, 2, 1,
        2, 1, 2
    });
    // Solution: x = [1, 1, 1] → Ax = [4, 6, 5]
    std::vector<double> b = {4, 6, 5};
    std::vector<double> x = QR::solve(A, b);

    REQUIRE(std::abs(x[0] - 1.0) < 1e-9);
    REQUIRE(std::abs(x[1] - 1.0) < 1e-9);
    REQUIRE(std::abs(x[2] - 1.0) < 1e-9);
}

TEST_CASE("QR solve: overdetermined 4x2 least-squares", "[qr]") {
    // Fit y = a + b*x through points (0,1), (1,2), (2,3), (3,4)
    // A = [[1,0],[1,1],[1,2],[1,3]], b = [1,2,3,4]
    // Exact solution: a=1, b=1
    Matrix A = make(4, 2, {
        1, 0,
        1, 1,
        1, 2,
        1, 3
    });
    std::vector<double> b = {1, 2, 3, 4};
    std::vector<double> x = QR::solve(A, b);

    REQUIRE(std::abs(x[0] - 1.0) < 1e-9);
    REQUIRE(std::abs(x[1] - 1.0) < 1e-9);
}

TEST_CASE("QR throws on m < n", "[qr]") {
    Matrix A(2, 3);
    REQUIRE_THROWS(QR::decompose(A));
}

TEST_CASE("QR throws on rank-deficient matrix", "[qr]") {
    // Rank 1 matrix (all rows are multiples of [1,2])
    Matrix A = make(3, 2, {
        1, 2,
        2, 4,
        3, 6
    });
    REQUIRE_THROWS(QR::decompose(A));
}

// ─── Newton Interpolation ─────────────────────────────────────────────────────

TEST_CASE("Newton matches Lagrange on 4-point data", "[interpolation]") {
    // Classic test: f(x) = x^3 sampled at x = 0,1,2,3
    std::vector<double> x = {0, 1, 2, 3};
    std::vector<double> y = {0, 1, 8, 27};
    double query = 1.5;

    double lag = Interpolation::lagrange(x, y, query);
    double newt = Interpolation::newton(x, y, query);

    REQUIRE(std::abs(newt - lag) < 1e-9);
}

TEST_CASE("Newton interpolates linear function exactly", "[interpolation]") {
    // f(x) = 2x + 3 — any interpolation must recover this exactly
    std::vector<double> x = {0, 1, 2, 4};
    std::vector<double> y = {3, 5, 7, 11};

    REQUIRE(std::abs(Interpolation::newton(x, y, 0.0) - 3.0)  < 1e-9);
    REQUIRE(std::abs(Interpolation::newton(x, y, 1.5) - 6.0)  < 1e-9);
    REQUIRE(std::abs(Interpolation::newton(x, y, 3.0) - 9.0)  < 1e-9);
}

TEST_CASE("Newton interpolates cubic polynomial exactly", "[interpolation]") {
    // f(x) = x^3 - 2x + 1; 4 data points determine it exactly
    std::vector<double> x = {-2, -1, 0, 1};
    std::vector<double> y = {-3,  2, 1, 0};  // f(-2)=-8+4+1=-3, f(-1)=-1+2+1=2, f(0)=1, f(1)=0

    // Verify at a known point: f(2) = 8 - 4 + 1 = 5
    REQUIRE(std::abs(Interpolation::newton(x, y, 2.0) - 5.0) < 1e-9);
}

TEST_CASE("Newton with single data point returns that value", "[interpolation]") {
    std::vector<double> x = {3.0};
    std::vector<double> y = {7.5};
    REQUIRE(std::abs(Interpolation::newton(x, y, 99.0) - 7.5) < 1e-9);
}

TEST_CASE("Newton throws on empty data", "[interpolation]") {
    std::vector<double> x, y;
    REQUIRE_THROWS(Interpolation::newton(x, y, 0.0));
}

TEST_CASE("Newton throws on size mismatch", "[interpolation]") {
    std::vector<double> x = {1, 2, 3};
    std::vector<double> y = {1, 2};  // wrong size
    REQUIRE_THROWS(Interpolation::newton(x, y, 0.0));
}

// ─── Inverse Power Method ────────────────────────────────────────────────────

TEST_CASE("Inverse power method: 2x2 known smallest eigenvalue", "[eigenpower]") {
    // A = [[2, 1], [1, 2]]: eigenvalues are 1 and 3; smallest = 1
    Matrix A = make(2, 2, {2, 1, 1, 2});
    std::vector<double> eigvec;
    double lambda = EigenPower::inverseMethod(A, eigvec);

    REQUIRE(std::abs(lambda - 1.0) < 1e-6);
}

TEST_CASE("Inverse power method: 3x3 diagonal — smallest eigenvalue", "[eigenpower]") {
    // Diagonal matrix: eigenvalues are exactly the diagonal entries
    // A = diag(5, 2, 8): smallest = 2
    Matrix A = make(3, 3, {
        5, 0, 0,
        0, 2, 0,
        0, 0, 8
    });
    std::vector<double> eigvec;
    double lambda = EigenPower::inverseMethod(A, eigvec);

    REQUIRE(std::abs(lambda - 2.0) < 1e-6);
}

TEST_CASE("Inverse power method: reports iteration count", "[eigenpower]") {
    Matrix A = make(2, 2, {2, 1, 1, 2});
    std::vector<double> eigvec;
    int iters = 0;
    double lambda = EigenPower::inverseMethod(A, eigvec, 1e-9, 1000, iters);

    REQUIRE(std::abs(lambda - 1.0) < 1e-6);
    REQUIRE(iters > 0);
    REQUIRE(iters < 1000);  // should converge well before limit
}

TEST_CASE("Inverse power method: smallest < largest (power method)", "[eigenpower]") {
    // A = [[4, 1], [1, 3]]: eigenvalues ≈ 4.618 and 2.382
    // inverse method gives smallest ≈ 2.382, power method gives largest ≈ 4.618
    Matrix A = make(2, 2, {4, 1, 1, 3});
    std::vector<double> eigvecInv, eigvecPow;

    double smallest = EigenPower::inverseMethod(A, eigvecInv);
    double largest  = EigenPower::powerMethod(A, eigvecPow);

    REQUIRE(smallest < largest);
    // Both should be positive for this SPD matrix
    REQUIRE(smallest > 0.0);
    REQUIRE(largest  > 0.0);
}

TEST_CASE("Inverse power method throws on non-square matrix", "[eigenpower]") {
    Matrix A(2, 3);
    std::vector<double> eigvec;
    REQUIRE_THROWS(EigenPower::inverseMethod(A, eigvec));
}
