"""Lagrange and Newton divided-difference interpolation."""

from typing import List


def lagrange(x: List[float], y: List[float], value: float) -> float:
    """Lagrange polynomial interpolation."""
    n = len(x)
    result = 0.0
    for i in range(n):
        term = y[i]
        for j in range(n):
            if j != i:
                term *= (value - x[j]) / (x[i] - x[j])
        result += term
    return result


def newton(x: List[float], y: List[float], value: float) -> float:
    """Newton divided-difference interpolation with Horner evaluation.

    Mathematically identical to lagrange() on same data.
    More efficient for multiple query points (O(n²) table build, O(n) per query).
    """
    n = len(x)
    if n == 0:
        raise ValueError("Newton interpolation: data points cannot be empty")
    if len(y) != n:
        raise ValueError("Newton interpolation: x and y must have the same length")

    # Build divided difference table in-place
    dd = list(map(float, y))
    for k in range(1, n):
        for i in range(n - 1, k - 1, -1):
            dd[i] = (dd[i] - dd[i - 1]) / (x[i] - x[i - k])

    # Horner evaluation (right to left)
    result = dd[n - 1]
    for i in range(n - 2, -1, -1):
        result = result * (value - x[i]) + dd[i]
    return result
