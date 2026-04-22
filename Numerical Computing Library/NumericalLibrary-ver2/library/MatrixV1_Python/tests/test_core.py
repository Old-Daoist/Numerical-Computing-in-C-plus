"""Tests for Matrix core class — mirrors C++ test_matrix.cpp (PLAN-002 cases)."""

import math
import pytest
from matrixv1.core.matrix import Matrix


TOL = 1e-9


def approx(a, b, tol=TOL):
    return abs(a - b) <= tol


# ── Construction ──────────────────────────────────────────────────────────────

def test_default_constructor_creates_zero_matrix():
    m = Matrix(0, 0)
    assert m.rows == 0 and m.cols == 0


def test_parameterised_constructor_sets_dimensions():
    m = Matrix(3, 4)
    assert m.rows == 3 and m.cols == 4


def test_1x1_constructor():
    m = Matrix(1, 1)
    assert m.rows == 1 and m.cols == 1


def test_elements_are_zero_initialised():
    m = Matrix(3, 3)
    for i in range(3):
        for j in range(3):
            assert m.get(i, j) == 0.0


def test_set_and_get_roundtrip():
    m = Matrix(2, 2)
    m.set(0, 1, 3.14)
    assert approx(m.get(0, 1), 3.14)


def test_set_overwrites_previous_value():
    m = Matrix(2, 2)
    m.set(1, 1, 5.0)
    m.set(1, 1, 9.9)
    assert approx(m.get(1, 1), 9.9)


# ── Predicates ────────────────────────────────────────────────────────────────

def test_is_square_true():
    assert Matrix(3, 3).is_square()


def test_is_square_false():
    assert not Matrix(2, 3).is_square()


def test_is_symmetric_true():
    m = Matrix.from_list([[4, 2], [2, 3]])
    assert m.is_symmetric()


def test_is_symmetric_false():
    m = Matrix.from_list([[1, 2], [3, 4]])
    assert not m.is_symmetric()


def test_is_symmetric_false_for_non_square():
    assert not Matrix(2, 3).is_symmetric()


def test_identity_is_symmetric():
    m = Matrix.from_list([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    assert m.is_symmetric()


# ── Bounds checking ───────────────────────────────────────────────────────────

def test_get_raises_on_negative_row():
    with pytest.raises(IndexError):
        Matrix(3, 3).get(-1, 0)


def test_get_raises_on_row_out_of_range():
    with pytest.raises(IndexError):
        Matrix(3, 3).get(3, 0)


def test_get_raises_on_negative_col():
    with pytest.raises(IndexError):
        Matrix(3, 3).get(0, -1)


def test_get_raises_on_col_out_of_range():
    with pytest.raises(IndexError):
        Matrix(3, 3).get(0, 3)


def test_set_raises_on_invalid_index():
    with pytest.raises(IndexError):
        Matrix(2, 2).set(5, 0, 1.0)


def test_boundary_indices_work():
    m = Matrix(3, 4)
    m.set(2, 3, 7.0)
    assert approx(m.get(2, 3), 7.0)


# ── Transpose ─────────────────────────────────────────────────────────────────

def test_transpose_2x3_gives_3x2():
    m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
    t = m.transpose()
    assert t.rows == 3 and t.cols == 2


def test_transpose_swaps_elements():
    m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
    t = m.transpose()
    assert approx(t.get(0, 0), 1) and approx(t.get(1, 0), 2) and approx(t.get(2, 1), 6)


def test_transpose_square():
    m = Matrix.from_list([[1, 2], [3, 4]])
    t = m.transpose()
    assert approx(t.get(0, 1), 3.0)


def test_double_transpose_equals_original():
    m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
    assert m.transpose().transpose() == m


# ── Determinant ───────────────────────────────────────────────────────────────

def test_det_1x1():
    assert approx(Matrix.from_list([[7.0]]).determinant(), 7.0)


def test_det_2x2_lu():
    m = Matrix.from_list([[3, 8], [4, 6]])
    assert approx(m.determinant(), 3 * 6 - 8 * 4)


def test_det_2x2_cofactor():
    m = Matrix.from_list([[3, 8], [4, 6]])
    assert approx(m.determinant("cofactor"), 3 * 6 - 8 * 4)


def test_det_3x3_both_methods_agree():
    m = Matrix.from_list([[6, 1, 1], [4, -2, 5], [2, 8, 7]])
    assert approx(m.determinant("lu"), m.determinant("cofactor"))


def test_det_singular_is_zero():
    m = Matrix.from_list([[1, 2], [2, 4]])
    assert approx(m.determinant(), 0.0)


def test_det_raises_on_non_square():
    with pytest.raises(ValueError):
        Matrix(2, 3).determinant()


def test_det_identity_is_1():
    m = Matrix.from_list([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    assert approx(m.determinant(), 1.0)


# ── Inverse ───────────────────────────────────────────────────────────────────

def test_inverse_2x2():
    m = Matrix.from_list([[4, 7], [2, 6]])
    inv = m.inverse()
    # Known inverse: 1/10 * [[6,-7],[-2,4]]
    assert approx(inv.get(0, 0), 0.6) and approx(inv.get(1, 1), 0.4)


def test_a_times_inverse_equals_identity():
    m = Matrix.from_list([[2, 1], [5, 3]])
    I_result = m * m.inverse()
    I = Matrix.from_list([[1, 0], [0, 1]])
    assert I_result == I


def test_inverse_of_identity_is_identity():
    I = Matrix.from_list([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    assert I.inverse() == I


def test_inverse_raises_on_singular():
    with pytest.raises(ValueError):
        Matrix.from_list([[1, 2], [2, 4]]).inverse()


def test_inverse_raises_on_non_square():
    with pytest.raises(ValueError):
        Matrix(2, 3).inverse()


# ── Operators ─────────────────────────────────────────────────────────────────

def test_add_produces_correct_result():
    A = Matrix.from_list([[1, 2], [3, 4]])
    B = Matrix.from_list([[5, 6], [7, 8]])
    C = A + B
    assert approx(C.get(0, 0), 6) and approx(C.get(1, 1), 12)


def test_add_raises_on_size_mismatch():
    with pytest.raises(ValueError):
        Matrix(2, 2) + Matrix(3, 3)


def test_subtract_produces_correct_result():
    A = Matrix.from_list([[5, 6], [7, 8]])
    B = Matrix.from_list([[1, 2], [3, 4]])
    C = A - B
    assert approx(C.get(0, 0), 4) and approx(C.get(1, 1), 4)


def test_multiply_produces_correct_result():
    A = Matrix.from_list([[1, 2], [3, 4]])
    B = Matrix.from_list([[2, 0], [1, 3]])
    C = A * B
    assert approx(C.get(0, 0), 4) and approx(C.get(1, 1), 12)


def test_multiply_raises_on_incompatible_dims():
    with pytest.raises(ValueError):
        Matrix(2, 3) * Matrix(2, 3)


def test_equality_true_for_equal_matrices():
    A = Matrix.from_list([[1, 2], [3, 4]])
    B = Matrix.from_list([[1, 2], [3, 4]])
    assert A == B


def test_equality_false_for_different_values():
    A = Matrix.from_list([[1, 2], [3, 4]])
    B = Matrix.from_list([[1, 2], [3, 5]])
    assert A != B


def test_equality_false_for_different_sizes():
    assert Matrix(2, 2) != Matrix(3, 3)


def test_from_list_builds_correctly():
    m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
    assert m.rows == 2 and m.cols == 3
    assert approx(m.get(1, 2), 6.0)
