from matrixv1.core.matrix import Matrix


def add(A: Matrix, B: Matrix) -> Matrix:
    if A.rows != B.rows or A.cols != B.cols:
        raise ValueError(f"Size mismatch: {A.rows}x{A.cols} vs {B.rows}x{B.cols}")
    result = Matrix(A.rows, A.cols)
    for i in range(A.rows):
        for j in range(A.cols):
            result.set(i, j, A.get(i, j) + B.get(i, j))
    return result
