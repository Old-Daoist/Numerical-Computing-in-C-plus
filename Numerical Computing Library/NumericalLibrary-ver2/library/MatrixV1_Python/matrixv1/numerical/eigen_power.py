"""Power method and inverse power method for eigenvalue computation."""

import math
from typing import List, Tuple
from matrixv1.core.matrix import Matrix
from matrixv1.solvers.lu_decomposition import solve as lu_solve


def power_method(
    A: Matrix,
    iterations: int = 100,
) -> Tuple[float, List[float]]:
    """Find the dominant (largest magnitude) eigenvalue via power iteration.

    Returns (eigenvalue, eigenvector).
    """
    if not A.is_square():
        raise ValueError("Power method: matrix must be square")
    n = A.rows
    v = [1.0] * n
    eigenvalue = 0.0

    for _ in range(iterations):
        # w = A * v
        w = [sum(A.get(i, j) * v[j] for j in range(n)) for i in range(n)]
        max_val = max(abs(wi) for wi in w)
        if max_val < 1e-12:
            raise ValueError("Power method: zero vector encountered")
        v = [wi / max_val for wi in w]
        eigenvalue = max_val

    return eigenvalue, v


def inverse_method(
    A: Matrix,
    tol: float = 1e-9,
    max_iter: int = 1000,
) -> Tuple[float, List[float], int]:
    """Find the smallest-magnitude eigenvalue via inverse power iteration.

    Each iteration solves A * w = v using LU (equivalent to A⁻¹ * v).
    Returns (eigenvalue, eigenvector, iterations_performed).
    """
    if not A.is_square():
        raise ValueError("Inverse power method: matrix must be square")
    n = A.rows

    # Initial guess: normalised [1, 2, 3, ..., n] (asymmetric to avoid degeneracy)
    norm = math.sqrt(sum((i + 1) ** 2 for i in range(n)))
    v = [(i + 1) / norm for i in range(n)]
    eigenvalue = 0.0

    for k in range(max_iter):
        try:
            w = lu_solve(A, v)
        except ValueError as e:
            raise ValueError(f"Inverse power method: LU solve failed — {e}") from e

        max_abs_w = max(abs(wi) for wi in w)
        if max_abs_w < 1e-12:
            raise ValueError("Inverse power method: zero vector encountered")

        eigenvalue = 1.0 / max_abs_w
        v_new = [wi / max_abs_w for wi in w]

        delta = math.sqrt(sum((v_new[i] - v[i]) ** 2 for i in range(n)))
        v = v_new

        if delta < tol:
            return eigenvalue, v, k + 1

    return eigenvalue, v, max_iter
