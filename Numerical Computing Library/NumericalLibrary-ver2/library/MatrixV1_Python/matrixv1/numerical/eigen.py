"""Gershgorin circle theorem — eigenvalue estimates."""

from matrixv1.core.matrix import Matrix


def gershgorin(A: Matrix) -> None:
    """Print Gershgorin disc estimates for eigenvalues of A."""
    if not A.is_square():
        raise ValueError("Gershgorin: matrix must be square")
    n = A.rows
    print("  Gershgorin eigenvalue estimates:")
    for i in range(n):
        centre = A.get(i, i)
        radius = sum(abs(A.get(i, j)) for j in range(n) if j != i)
        print(f"    Row {i}: centre = {centre:.6g},  radius = {radius:.6g}"
              f"  →  λ ∈ [{centre - radius:.6g}, {centre + radius:.6g}]")
