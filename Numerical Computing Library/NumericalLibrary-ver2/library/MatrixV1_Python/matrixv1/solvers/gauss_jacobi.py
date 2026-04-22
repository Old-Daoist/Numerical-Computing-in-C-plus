"""Gauss-Jacobi iterative solver."""

import math
from typing import List, Tuple
from matrixv1.core.matrix import Matrix


def solve(
    A: Matrix,
    b: List[float],
    tol: float = 1e-6,
    max_iter: int = 1000,
) -> Tuple[List[float], int]:
    """Solve Ax = b via Jacobi iteration.

    Returns (solution, iterations_performed).
    Raises ValueError if any diagonal element is zero.
    """
    n = A.rows
    for i in range(n):
        if abs(A.get(i, i)) < 1e-12:
            raise ValueError(f"Zero diagonal at row {i} — Jacobi method invalid")

    x = [0.0] * n

    for k in range(max_iter):
        x_new = [0.0] * n
        for i in range(n):
            total = b[i] - sum(
                A.get(i, j) * x[j] for j in range(n) if j != i
            )
            x_new[i] = total / A.get(i, i)

        delta = math.sqrt(sum((x_new[i] - x[i]) ** 2 for i in range(n)))
        x = x_new

        if delta < tol:
            return x, k + 1

    return x, max_iter
