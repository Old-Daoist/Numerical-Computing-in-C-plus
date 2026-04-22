from matrixv1.core.matrix import Matrix
from matrixv1.operations.multiply import multiply


def divide(A: Matrix, B: Matrix) -> Matrix:
    """Compute A * inv(B). B must be square and invertible."""
    if A.cols != B.rows:
        raise ValueError(
            "Matrix dimensions incompatible for division: A.cols must equal B.rows"
        )
    return multiply(A, B.inverse())
