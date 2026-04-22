"""Tests for matrix operations — Add, Subtract, Multiply, Divide."""

import pytest
from matrixv1.core.matrix import Matrix
from matrixv1.operations.add import add
from matrixv1.operations.subtract import subtract
from matrixv1.operations.multiply import multiply
from matrixv1.operations.divide import divide

TOL = 1e-9


def approx(a, b):
    return abs(a - b) <= TOL


# ── Add ───────────────────────────────────────────────────────────────────────

def test_add_2x2():
    A = Matrix.from_list([[1, 2], [3, 4]])
    B = Matrix.from_list([[5, 6], [7, 8]])
    C = add(A, B)
    assert approx(C.get(0, 0), 6) and approx(C.get(1, 1), 12)


def test_add_size_mismatch_raises():
    with pytest.raises(ValueError):
        add(Matrix(2, 2), Matrix(3, 3))


def test_add_zero_matrix():
    A = Matrix.from_list([[1, 2], [3, 4]])
    Z = Matrix(2, 2)
    assert add(A, Z) == A


# ── Subtract ─────────────────────────────────────────────────────────────────

def test_subtract_2x2():
    A = Matrix.from_list([[5, 6], [7, 8]])
    B = Matrix.from_list([[1, 2], [3, 4]])
    C = subtract(A, B)
    assert approx(C.get(0, 0), 4) and approx(C.get(1, 1), 4)


def test_subtract_size_mismatch_raises():
    with pytest.raises(ValueError):
        subtract(Matrix(2, 2), Matrix(2, 3))


def test_subtract_self_gives_zero():
    A = Matrix.from_list([[3, 1], [2, 5]])
    Z = subtract(A, A)
    assert Z == Matrix(2, 2)


# ── Multiply ──────────────────────────────────────────────────────────────────

def test_multiply_2x2():
    A = Matrix.from_list([[1, 2], [3, 4]])
    B = Matrix.from_list([[2, 0], [1, 3]])
    C = multiply(A, B)
    assert approx(C.get(0, 0), 4) and approx(C.get(0, 1), 6)
    assert approx(C.get(1, 0), 10) and approx(C.get(1, 1), 12)


def test_multiply_rect():
    A = Matrix.from_list([[1, 2, 3], [4, 5, 6]])  # 2×3
    B = Matrix.from_list([[7], [8], [9]])           # 3×1
    C = multiply(A, B)
    assert C.rows == 2 and C.cols == 1
    assert approx(C.get(0, 0), 50) and approx(C.get(1, 0), 122)


def test_multiply_incompatible_raises():
    with pytest.raises(ValueError):
        multiply(Matrix(2, 3), Matrix(2, 3))


def test_multiply_by_identity():
    A = Matrix.from_list([[3, 1], [2, 5]])
    I = Matrix.from_list([[1, 0], [0, 1]])
    assert multiply(A, I) == A


# ── Divide ────────────────────────────────────────────────────────────────────

def test_divide_2x2():
    A = Matrix.from_list([[3, 0], [0, 3]])
    B = Matrix.from_list([[1, 0], [0, 1]])   # B = I → A * inv(I) = A
    assert divide(A, B) == A


def test_divide_3x3():
    A = Matrix.from_list([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    B = Matrix.from_list([[2, 0, 0], [0, 2, 0], [0, 0, 2]])
    C = divide(A, B)   # A * inv(2I) = 0.5*I
    assert abs(C.get(0, 0) - 0.5) < 1e-9


def test_divide_singular_raises():
    with pytest.raises(ValueError):
        divide(Matrix.from_list([[1, 0], [0, 1]]),
               Matrix.from_list([[1, 2], [2, 4]]))


def test_divide_dim_mismatch_raises():
    with pytest.raises(ValueError):
        divide(Matrix(2, 3), Matrix(2, 2))
