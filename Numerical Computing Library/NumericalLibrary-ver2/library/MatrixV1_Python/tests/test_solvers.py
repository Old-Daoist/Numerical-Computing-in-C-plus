"""Tests for all solvers — mirrors C++ test_matrix.cpp solver cases."""

import math
import pytest
from matrixv1.core.matrix import Matrix
from matrixv1.solvers.gauss_elimination import solve as gauss_solve
from matrixv1.solvers.lu_decomposition import decompose as lu_decompose, solve as lu_solve
from matrixv1.solvers.cholesky import decompose as cholesky_decompose, solve as cholesky_solve
from matrixv1.solvers.qr import decompose as qr_decompose, solve as qr_solve
from matrixv1.solvers.gauss_jacobi import solve as jacobi_solve
from matrixv1.solvers.gauss_seidel import solve as seidel_solve

TOL = 1e-6


def vec_close(a, b, tol=TOL):
    return all(abs(x - y) <= tol for x, y in zip(a, b))


# ── Gauss Elimination ─────────────────────────────────────────────────────────

def test_gauss_2x2_known_solution():
    A = Matrix.from_list([[2, 1], [5, 3]])
    b = [8, 19]
    x = gauss_solve(A, b)
    assert vec_close(x, [5.0, -2.0])


def test_gauss_3x3():
    A = Matrix.from_list([[3, 2, -1], [2, -2, 4], [-1, 0.5, -1]])
    b = [1, -2, 0]
    x = gauss_solve(A, b)
    # Verify Ax ≈ b
    for i in range(3):
        assert abs(sum(A.get(i, j) * x[j] for j in range(3)) - b[i]) < 1e-8


def test_gauss_singular_raises():
    with pytest.raises(ValueError):
        gauss_solve(Matrix.from_list([[1, 2], [2, 4]]), [1, 2])


def test_gauss_identity_system():
    A = Matrix.from_list([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    b = [3, 7, 2]
    x = gauss_solve(A, b)
    assert vec_close(x, b)


# ── LU Decomposition ─────────────────────────────────────────────────────────

def test_lu_decompose_gives_correct_L_U():
    A = Matrix.from_list([[2, 3], [4, 7]])
    L, U = lu_decompose(A)
    # Verify A = L * U
    LU = L * U
    assert LU == A


def test_lu_decompose_L_has_unit_diagonal():
    A = Matrix.from_list([[4, 3], [6, 3]])
    L, _ = lu_decompose(A)
    assert abs(L.get(0, 0) - 1.0) < 1e-12
    assert abs(L.get(1, 1) - 1.0) < 1e-12


def test_lu_solve_2x2():
    A = Matrix.from_list([[2, 1], [5, 3]])
    b = [8, 19]
    x = lu_solve(A, b)
    assert vec_close(x, [5.0, -2.0])


def test_lu_solve_3x3():
    A = Matrix.from_list([[1, 1, 0], [2, 1, -1], [3, -1, -1]])
    b = [2, 0, -1]
    x = lu_solve(A, b)
    for i in range(3):
        assert abs(sum(A.get(i, j) * x[j] for j in range(3)) - b[i]) < 1e-8


# ── Cholesky ──────────────────────────────────────────────────────────────────

def test_cholesky_decompose_2x2():
    A = Matrix.from_list([[4, 2], [2, 3]])
    L = cholesky_decompose(A)
    assert abs(L.get(0, 0) - 2.0) < 1e-9
    assert abs(L.get(1, 1) - math.sqrt(2)) < 1e-9


def test_cholesky_verify_A_equals_L_LT():
    A = Matrix.from_list([[6, 3, 4], [3, 6, 5], [4, 5, 10]])
    L = cholesky_decompose(A)
    LLT = L * L.transpose()
    assert LLT == A


def test_cholesky_solve_known_spd():
    A = Matrix.from_list([[4, 2], [2, 3]])
    b = [6, 5]
    x = cholesky_solve(A, b)
    for i in range(2):
        assert abs(sum(A.get(i, j) * x[j] for j in range(2)) - b[i]) < 1e-9


def test_cholesky_identity():
    I = Matrix.from_list([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    L = cholesky_decompose(I)
    assert L == I


def test_cholesky_throws_non_spd():
    with pytest.raises(ValueError):
        cholesky_decompose(Matrix.from_list([[1, 2], [2, 1]]))


def test_cholesky_throws_non_square():
    with pytest.raises(ValueError):
        cholesky_decompose(Matrix(2, 3))


def test_cholesky_solve_4x4():
    A = Matrix.from_list([
        [4, 2, 2, 0],
        [2, 5, 3, 1],
        [2, 3, 6, 2],
        [0, 1, 2, 4],
    ])
    b = [1, 1, 1, 1]
    x = cholesky_solve(A, b)
    for i in range(4):
        assert abs(sum(A.get(i, j) * x[j] for j in range(4)) - b[i]) < 1e-8


# ── QR ────────────────────────────────────────────────────────────────────────

def test_qr_verify_A_equals_QR():
    A = Matrix.from_list([[1, 2], [3, 4], [5, 6]])
    qr = qr_decompose(A)
    QR = qr.Q * qr.R
    # Compare with A (m×n result)
    for i in range(A.rows):
        for j in range(A.cols):
            assert abs(QR.get(i, j) - A.get(i, j)) < 1e-9


def test_qr_Q_is_orthogonal():
    A = Matrix.from_list([[1, 2], [3, 4], [5, 6]])
    qr = qr_decompose(A)
    QTQ = qr.Q.transpose() * qr.Q
    I = Matrix.from_list([[1, 0], [0, 1]])
    assert QTQ == I


def test_qr_R_is_upper_triangular():
    A = Matrix.from_list([[12, -51, 4], [6, 167, -68], [-4, 24, -41]])
    qr = qr_decompose(A)
    for i in range(qr.R.rows):
        for j in range(i):
            assert abs(qr.R.get(i, j)) < 1e-9


def test_qr_solve_square():
    A = Matrix.from_list([[3, 2], [1, 4]])
    b = [8, 9]
    x = qr_solve(A, b)
    for i in range(2):
        assert abs(sum(A.get(i, j) * x[j] for j in range(2)) - b[i]) < 1e-8


def test_qr_solve_overdetermined():
    # Least-squares: overdetermined 3×2 system
    A = Matrix.from_list([[1, 1], [1, 2], [1, 3]])
    b = [1, 2, 3]
    x = qr_solve(A, b)
    assert len(x) == 2  # returns 2-vector solution


def test_qr_throws_m_lt_n():
    with pytest.raises(ValueError):
        qr_decompose(Matrix(2, 4))


def test_qr_throws_rank_deficient():
    with pytest.raises(ValueError):
        qr_decompose(Matrix.from_list([[1, 2], [2, 4], [3, 6]]))


# ── Gauss-Jacobi ──────────────────────────────────────────────────────────────

_diag_dominant = Matrix.from_list([
    [10, 1, 1],
    [1, 10, 1],
    [1, 1, 10],
])
_rhs = [12, 12, 12]  # solution is [1, 1, 1]


def test_jacobi_converges():
    x, iters = jacobi_solve(_diag_dominant, _rhs, tol=1e-6, max_iter=1000)
    assert iters < 1000
    assert vec_close(x, [1.0, 1.0, 1.0])


def test_jacobi_tighter_tol_needs_more_iters():
    _, iters_loose = jacobi_solve(_diag_dominant, _rhs, tol=1e-2, max_iter=1000)
    _, iters_tight = jacobi_solve(_diag_dominant, _rhs, tol=1e-8, max_iter=1000)
    assert iters_tight >= iters_loose


def test_jacobi_hits_max_iter():
    _, iters = jacobi_solve(_diag_dominant, _rhs, tol=1e-15, max_iter=5)
    assert iters == 5


def test_jacobi_zero_diagonal_raises():
    A = Matrix.from_list([[0, 1], [1, 0]])
    with pytest.raises(ValueError):
        jacobi_solve(A, [1, 1])


# ── Gauss-Seidel ──────────────────────────────────────────────────────────────

def test_seidel_converges():
    x, iters = seidel_solve(_diag_dominant, _rhs, tol=1e-6, max_iter=1000)
    assert iters < 1000
    assert vec_close(x, [1.0, 1.0, 1.0])


def test_seidel_converges_faster_than_jacobi():
    _, j_iters = jacobi_solve(_diag_dominant, _rhs, tol=1e-8, max_iter=1000)
    _, s_iters = seidel_solve(_diag_dominant, _rhs, tol=1e-8, max_iter=1000)
    assert s_iters <= j_iters


def test_seidel_hits_max_iter():
    _, iters = seidel_solve(_diag_dominant, _rhs, tol=1e-15, max_iter=5)
    assert iters == 5


def test_seidel_zero_diagonal_raises():
    A = Matrix.from_list([[0, 1], [1, 2]])
    with pytest.raises(ValueError):
        seidel_solve(A, [1, 1])
