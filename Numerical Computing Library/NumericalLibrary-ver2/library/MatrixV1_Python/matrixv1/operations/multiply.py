from matrixv1.core.matrix import Matrix


def multiply(A: Matrix, B: Matrix) -> Matrix:
    if A.cols != B.rows:
        raise ValueError(
            f"Invalid dimensions for multiply: {A.rows}x{A.cols} * {B.rows}x{B.cols}"
        )
    result = Matrix(A.rows, B.cols)
    for i in range(A.rows):
        for j in range(B.cols):
            total = sum(A.get(i, k) * B.get(k, j) for k in range(A.cols))
            result.set(i, j, total)
    return result
