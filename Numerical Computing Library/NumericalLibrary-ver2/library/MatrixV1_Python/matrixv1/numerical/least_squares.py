"""Least squares curve fitting — straight line and parabola.

Both functions build the normal-equation system from single-pass sums of
powers of x and mixed x*y products, then solve the resulting small linear
system using GaussElimination.solve().  The algorithm exactly mirrors the
C++ LeastSquares implementation in MatrixV1.
"""

from typing import List, Tuple
from matrixv1.core.matrix import Matrix
from matrixv1.solvers.gauss_elimination import solve as gauss_solve


def fit_line(x: List[float], y: List[float]) -> Tuple[float, float]:
    """Fit  y = a + b*x  to data points by least squares.

    Minimises Σ(yᵢ - a - b·xᵢ)² and solves the resulting 2×2 normal
    equation system:

        [ n     Σx  ] [a]   [Σy  ]
        [ Σx    Σx² ] [b] = [Σxy ]

    Parameters
    ----------
    x, y : lists of floats (same length, at least 2 elements)

    Returns
    -------
    (a, b) — intercept and slope of the best-fit line

    Raises
    ------
    ValueError
        If x/y are empty, have different lengths, fewer than 2 points, or
        all x values are identical (singular normal-equation matrix).
    """
    n = len(x)
    if n == 0 or len(y) != n:
        raise ValueError(
            "fit_line: x and y must be non-empty and the same length")
    if n < 2:
        raise ValueError(
            "fit_line: at least 2 data points are required")

    # Single-pass accumulation
    sx = sx2 = sy = sxy = 0.0
    for xi, yi in zip(x, y):
        sx  += xi
        sx2 += xi * xi
        sy  += yi
        sxy += xi * yi

    # Build 2×2 normal-equation matrix
    A = Matrix(2, 2)
    A.set(0, 0, float(n));  A.set(0, 1, sx)
    A.set(1, 0, sx);        A.set(1, 1, sx2)
    rhs = [sy, sxy]

    try:
        coeffs = gauss_solve(A, rhs)
    except RuntimeError:
        raise ValueError(
            "fit_line: normal-equation matrix is singular "
            "(all x values may be identical)")

    return (coeffs[0], coeffs[1])


def fit_parabola(x: List[float], y: List[float]) -> Tuple[float, float, float]:
    """Fit  y = a + b*x + c*x²  to data points by least squares.

    Minimises Σ(yᵢ - a - b·xᵢ - c·xᵢ²)² and solves the resulting 3×3
    normal equation system:

        [ n     Σx    Σx²  ] [a]   [Σy   ]
        [ Σx    Σx²   Σx³  ] [b] = [Σxy  ]
        [ Σx²   Σx³   Σx⁴  ] [c]   [Σx²y ]

    Parameters
    ----------
    x, y : lists of floats (same length, at least 3 elements)

    Returns
    -------
    (a, b, c) — intercept, linear, and quadratic coefficients

    Raises
    ------
    ValueError
        If x/y are empty, have different lengths, fewer than 3 points, or
        the normal-equation matrix is singular.
    """
    n = len(x)
    if n == 0 or len(y) != n:
        raise ValueError(
            "fit_parabola: x and y must be non-empty and the same length")
    if n < 3:
        raise ValueError(
            "fit_parabola: at least 3 data points are required")

    # Single-pass accumulation
    sx = sx2 = sx3 = sx4 = 0.0
    sy = sxy = sx2y = 0.0
    for xi, yi in zip(x, y):
        xi2  = xi * xi
        xi3  = xi2 * xi
        xi4  = xi2 * xi2
        sx   += xi
        sx2  += xi2
        sx3  += xi3
        sx4  += xi4
        sy   += yi
        sxy  += xi  * yi
        sx2y += xi2 * yi

    # Build 3×3 normal-equation matrix
    A = Matrix(3, 3)
    A.set(0, 0, float(n)); A.set(0, 1, sx);  A.set(0, 2, sx2)
    A.set(1, 0, sx);       A.set(1, 1, sx2); A.set(1, 2, sx3)
    A.set(2, 0, sx2);      A.set(2, 1, sx3); A.set(2, 2, sx4)
    rhs = [sy, sxy, sx2y]

    try:
        coeffs = gauss_solve(A, rhs)
    except RuntimeError:
        raise ValueError(
            "fit_parabola: normal-equation matrix is singular "
            "(degenerate x distribution)")

    return (coeffs[0], coeffs[1], coeffs[2])
