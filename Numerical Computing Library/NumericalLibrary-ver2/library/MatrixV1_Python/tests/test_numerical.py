"""Tests for numerical methods — interpolation and eigenvalue methods."""

import math
import pytest
from matrixv1.core.matrix import Matrix
from matrixv1.numerical.interpolation import lagrange, newton
from matrixv1.numerical.eigen_power import power_method, inverse_method

TOL = 1e-9


def approx(a, b, tol=TOL):
    return abs(a - b) <= tol


# ── Lagrange Interpolation ────────────────────────────────────────────────────

def test_lagrange_linear():
    x = [0.0, 1.0]
    y = [0.0, 2.0]  # f(x) = 2x
    assert approx(lagrange(x, y, 0.5), 1.0)


def test_lagrange_quadratic():
    x = [0.0, 1.0, 2.0]
    y = [0.0, 1.0, 4.0]  # f(x) = x²
    assert approx(lagrange(x, y, 1.5), 2.25)


def test_lagrange_at_node_returns_node_value():
    x = [1.0, 2.0, 3.0]
    y = [1.0, 4.0, 9.0]
    assert approx(lagrange(x, y, 2.0), 4.0)


# ── Newton Interpolation ──────────────────────────────────────────────────────

def test_newton_matches_lagrange_4_points():
    x = [0.0, 1.0, 2.0, 3.0]
    y = [0.0, 1.0, 8.0, 27.0]  # f(x) = x³
    for v in [0.5, 1.5, 2.5]:
        assert approx(newton(x, y, v), lagrange(x, y, v))


def test_newton_linear_exact():
    x = [0.0, 1.0]
    y = [3.0, 5.0]  # f(x) = 2x + 3
    assert approx(newton(x, y, 0.5), 4.0)
    assert approx(newton(x, y, 2.0), 7.0)


def test_newton_cubic_exact():
    x = [0.0, 1.0, 2.0, 3.0]
    y = [1.0, 0.0, 1.0, 10.0]  # f(x) = x³ - 2x + 1 at these points
    assert approx(newton(x, y, 0.0), 1.0)
    assert approx(newton(x, y, 1.0), 0.0)


def test_newton_single_point():
    assert approx(newton([5.0], [3.14], 99.0), 3.14)


def test_newton_raises_on_empty():
    with pytest.raises(ValueError):
        newton([], [], 1.0)


def test_newton_raises_on_size_mismatch():
    with pytest.raises(ValueError):
        newton([1.0, 2.0], [1.0], 1.5)


# ── Power Method ─────────────────────────────────────────────────────────────

def test_power_method_2x2():
    # [[2, 1], [1, 2]] has eigenvalues 3 and 1; dominant = 3
    A = Matrix.from_list([[2, 1], [1, 2]])
    lam, _ = power_method(A, iterations=200)
    assert approx(lam, 3.0, tol=1e-5)


def test_power_method_diagonal():
    # Diagonal matrix: dominant eigenvalue = 8
    A = Matrix.from_list([[5, 0, 0], [0, 2, 0], [0, 0, 8]])
    lam, _ = power_method(A, iterations=200)
    assert approx(lam, 8.0, tol=1e-4)


def test_power_method_raises_on_non_square():
    with pytest.raises(ValueError):
        power_method(Matrix(2, 3))


# ── Inverse Power Method ──────────────────────────────────────────────────────

def test_inverse_method_2x2():
    # [[2, 1], [1, 2]]: smallest eigenvalue = 1
    A = Matrix.from_list([[2, 1], [1, 2]])
    lam, _, iters = inverse_method(A, tol=1e-9, max_iter=1000)
    assert approx(lam, 1.0, tol=1e-6)


def test_inverse_method_diagonal():
    # diag(5, 2, 8): smallest = 2
    A = Matrix.from_list([[5, 0, 0], [0, 2, 0], [0, 0, 8]])
    lam, _, _ = inverse_method(A)
    assert approx(lam, 2.0, tol=1e-6)


def test_inverse_method_reports_iters():
    A = Matrix.from_list([[4, 1], [2, 3]])
    _, _, iters = inverse_method(A, tol=1e-9, max_iter=1000)
    assert iters > 0 and iters < 1000


def test_inverse_smaller_than_power():
    A = Matrix.from_list([[2, 1], [1, 2]])
    lam_dom, _ = power_method(A, iterations=300)
    lam_min, _, _ = inverse_method(A)
    assert lam_min < lam_dom


def test_inverse_method_raises_on_non_square():
    with pytest.raises(ValueError):
        inverse_method(Matrix(2, 3))


# ── Least Squares — fit_line ──────────────────────────────────────────────────

from matrixv1.numerical.least_squares import fit_line, fit_parabola


def test_fit_line_classic_5_points():
    # Standard OLS result: a=2.2, b=0.6
    x = [1, 2, 3, 4, 5]
    y = [2, 4, 5, 4, 5]
    a, b = fit_line(x, y)
    assert approx(a, 2.2)
    assert approx(b, 0.6)


def test_fit_line_exact_fit():
    # y = 3 + 2x exactly
    x = [0, 1, 2, 3, 4]
    y = [3, 5, 7, 9, 11]
    a, b = fit_line(x, y)
    assert approx(a, 3.0)
    assert approx(b, 2.0)


def test_fit_line_negative_slope():
    # y = 10 - 3x
    x = [0, 1, 2, 3]
    y = [10, 7, 4, 1]
    a, b = fit_line(x, y)
    assert approx(a, 10.0)
    assert approx(b, -3.0)


def test_fit_line_two_points():
    x = [0, 2]
    y = [1, 5]
    a, b = fit_line(x, y)
    assert approx(a, 1.0)
    assert approx(b, 2.0)


def test_fit_line_horizontal():
    x = [1, 2, 3, 4, 5]
    y = [4, 4, 4, 4, 4]
    a, b = fit_line(x, y)
    assert approx(b, 0.0)
    assert approx(a, 4.0)


def test_fit_line_raises_empty():
    with pytest.raises(ValueError):
        fit_line([], [])


def test_fit_line_raises_mismatch():
    with pytest.raises(ValueError):
        fit_line([1, 2, 3], [1, 2])


def test_fit_line_raises_too_few_points():
    with pytest.raises(ValueError):
        fit_line([1.0], [2.0])


def test_fit_line_raises_singular():
    # All x identical → singular normal matrix
    with pytest.raises(ValueError):
        fit_line([2, 2, 2], [1, 2, 3])


# ── Least Squares — fit_parabola ──────────────────────────────────────────────

def test_fit_parabola_exact_fit():
    # y = 1 + 2x + 3x²
    x = [0, 1, 2, 3, 4]
    y = [1, 6, 17, 34, 57]
    a, b, c = fit_parabola(x, y)
    assert approx(a, 1.0)
    assert approx(b, 2.0)
    assert approx(c, 3.0)


def test_fit_parabola_noisy_data():
    # Data: {0,1,2,3} vs {1,1.5,3.5,8.0}
    # OLS: a=1.05, b=-0.7, c=1.0 (verified via numpy.polyfit)
    x = [0, 1, 2, 3]
    y = [1, 1.5, 3.5, 8.0]
    a, b, c = fit_parabola(x, y)
    assert approx(a, 1.05)
    assert approx(b, -0.7)
    assert approx(c, 1.0)


def test_fit_parabola_zero_quadratic():
    # Linear data — c must be ~0
    x = [0, 1, 2, 3, 4]
    y = [2, 7, 12, 17, 22]
    a, b, c = fit_parabola(x, y)
    assert approx(a, 2.0)
    assert approx(b, 5.0)
    assert approx(c, 0.0)


def test_fit_parabola_three_points_unique():
    # y = x² exactly from 3 points
    x = [0, 1, 2]
    y = [0, 1, 4]
    a, b, c = fit_parabola(x, y)
    assert approx(a, 0.0)
    assert approx(b, 0.0)
    assert approx(c, 1.0)


def test_fit_parabola_raises_empty():
    with pytest.raises(ValueError):
        fit_parabola([], [])


def test_fit_parabola_raises_mismatch():
    with pytest.raises(ValueError):
        fit_parabola([1, 2, 3], [1, 2])


def test_fit_parabola_raises_too_few_points():
    with pytest.raises(ValueError):
        fit_parabola([1, 2], [3, 4])
