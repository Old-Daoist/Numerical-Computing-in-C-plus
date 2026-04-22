"""Cholesky decomposition for symmetric positive-definite matrices."""

import math
from typing import List
from matrixv1.core.matrix import Matrix


def decompose(A: Matrix) -> Matrix:
    """Decompose SPD matrix A into L where A = L * L^T.

    Raises ValueError if A is not square or not positive-definite.
    """
    if not A.is_square():
        raise ValueError("Cholesky: matrix must be square")
    n = A.rows
    L = Matrix(n, n)

    for i in range(n):
        for j in range(i + 1):
            if i == j:
                val = A.get(i, i) - sum(L.get(i, k) ** 2 for k in range(j))
                if val <= 1e-12:
                    raise ValueError(
                        "Cholesky: matrix is not positive-definite"
                    )
                L.set(i, i, math.sqrt(val))
            else:
                val = A.get(i, j) - sum(
                    L.get(i, k) * L.get(j, k) for k in range(j)
                )
                l_jj = L.get(j, j)
                if abs(l_jj) < 1e-12:
                    raise ValueError("Cholesky: zero pivot encountered")
                L.set(i, j, val / l_jj)
    return L


def solve(A: Matrix, b: List[float]) -> List[float]:
    """Solve Ax = b using Cholesky decomposition (A must be SPD)."""
    L = decompose(A)
    n = A.rows

    # Forward substitution Ly = b
    y = [0.0] * n
    for i in range(n):
        y[i] = (b[i] - sum(L.get(i, j) * y[j] for j in range(i))) / L.get(i, i)

    # Backward substitution L^T x = y  (L^T[i][j] = L[j][i])
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        x[i] = (y[i] - sum(L.get(j, i) * x[j] for j in range(i + 1, n))) / L.get(i, i)
    return x
