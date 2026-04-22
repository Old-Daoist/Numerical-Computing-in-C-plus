"""Gauss elimination with partial pivoting."""

import copy
from typing import List
from matrixv1.core.matrix import Matrix


def solve(A: Matrix, b: List[float]) -> List[float]:
    """Solve Ax = b via Gauss elimination with partial pivoting."""
    n = A.rows
    mat = A._raw()
    rhs = list(b)

    for i in range(n):
        # Partial pivot
        pivot = max(range(i, n), key=lambda p: abs(mat[p][i]))
        if abs(mat[pivot][i]) < 1e-12:
            raise ValueError("Singular matrix — zero pivot encountered")
        if pivot != i:
            mat[i], mat[pivot] = mat[pivot], mat[i]
            rhs[i], rhs[pivot] = rhs[pivot], rhs[i]
        # Elimination
        for j in range(i + 1, n):
            factor = mat[j][i] / mat[i][i]
            for k in range(i, n):
                mat[j][k] -= factor * mat[i][k]
            rhs[j] -= factor * rhs[i]

    # Back substitution
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        if abs(mat[i][i]) < 1e-12:
            raise ValueError("Singular matrix in back substitution")
        x[i] = rhs[i]
        for j in range(i + 1, n):
            x[i] -= mat[i][j] * x[j]
        x[i] /= mat[i][i]
    return x
