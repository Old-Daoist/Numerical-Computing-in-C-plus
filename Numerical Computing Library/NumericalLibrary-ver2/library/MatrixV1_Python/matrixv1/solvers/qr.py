"""QR decomposition using Modified Gram-Schmidt."""

import math
from typing import List, Tuple, NamedTuple
from matrixv1.core.matrix import Matrix


class QRDecomposition(NamedTuple):
    Q: Matrix  # m×n orthogonal columns
    R: Matrix  # n×n upper triangular


def decompose(A: Matrix) -> QRDecomposition:
    """Decompose A (m×n, m≥n) into Q, R where A = Q * R.

    Uses Modified Gram-Schmidt for numerical stability.
    Raises ValueError if m < n or matrix is rank-deficient.
    """
    m, n = A.rows, A.cols
    if m < n:
        raise ValueError("QR: matrix must have m >= n (more rows than columns)")

    # Working columns as lists
    W = [[A.get(i, j) for i in range(m)] for j in range(n)]
    Q = Matrix(m, n)
    R = Matrix(n, n)

    for j in range(n):
        col = list(W[j])

        # Modified Gram-Schmidt: orthogonalise against previous Q columns
        for k in range(j):
            q_k = [Q.get(i, k) for i in range(m)]
            r_kj = sum(q_k[i] * col[i] for i in range(m))
            R.set(k, j, r_kj)
            for i in range(m):
                col[i] -= r_kj * q_k[i]

        norm = math.sqrt(sum(v ** 2 for v in col))
        if norm < 1e-12:
            raise ValueError(f"QR: matrix appears rank-deficient at column {j}")

        R.set(j, j, norm)
        for i in range(m):
            Q.set(i, j, col[i] / norm)

    return QRDecomposition(Q, R)


def solve(A: Matrix, b: List[float]) -> List[float]:
    """Solve Ax = b (exact for square, least-squares for overdetermined)."""
    m, n = A.rows, A.cols
    if len(b) != m:
        raise ValueError("QR solve: b must have m elements")

    qr = decompose(A)

    # c = Q^T * b
    c = [sum(qr.Q.get(i, k) * b[i] for i in range(m)) for k in range(n)]

    # Back-substitute R x = c
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        r_ii = qr.R.get(i, i)
        if abs(r_ii) < 1e-12:
            raise ValueError("QR solve: singular R diagonal encountered")
        x[i] = (c[i] - sum(qr.R.get(i, j) * x[j] for j in range(i + 1, n))) / r_ii
    return x
