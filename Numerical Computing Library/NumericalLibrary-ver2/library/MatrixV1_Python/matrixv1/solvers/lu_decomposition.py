"""LU decomposition (Doolittle) — no pivoting."""

from typing import List, Tuple
from matrixv1.core.matrix import Matrix


def decompose(A: Matrix) -> Tuple[Matrix, Matrix]:
    """Decompose A into L, U such that A = L * U (Doolittle method)."""
    n = A.rows
    L = Matrix(n, n)
    U = Matrix(n, n)

    for i in range(n):
        # Upper triangular U
        for k in range(i, n):
            total = sum(L.get(i, j) * U.get(j, k) for j in range(i))
            U.set(i, k, A.get(i, k) - total)
        # Lower triangular L
        for k in range(i, n):
            if i == k:
                L.set(i, i, 1.0)
            else:
                total = sum(L.get(k, j) * U.get(j, i) for j in range(i))
                if abs(U.get(i, i)) < 1e-12:
                    raise ValueError("Zero pivot in LU decomposition")
                L.set(k, i, (A.get(k, i) - total) / U.get(i, i))
    return L, U


def solve(A: Matrix, b: List[float]) -> List[float]:
    """Solve Ax = b via LU decomposition."""
    n = A.rows
    L, U = decompose(A)

    # Forward substitution Ly = b
    y = [0.0] * n
    for i in range(n):
        y[i] = b[i] - sum(L.get(i, j) * y[j] for j in range(i))

    # Backward substitution Ux = y
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        x[i] = y[i] - sum(U.get(i, j) * x[j] for j in range(i + 1, n))
        if abs(U.get(i, i)) < 1e-12:
            raise ValueError("Zero pivot in LU back substitution")
        x[i] /= U.get(i, i)
    return x
