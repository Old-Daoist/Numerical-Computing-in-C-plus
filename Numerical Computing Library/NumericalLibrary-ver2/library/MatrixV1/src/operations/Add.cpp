#include "../../include/operations/Add.hpp"
#include <stdexcept>

Matrix Add::compute(const Matrix& A, const Matrix& B) {
    if(A.getRows() != B.getRows() || A.getCols() != B.getCols())
        throw std::invalid_argument("Size mismatch");

    Matrix result(A.getRows(), A.getCols());

    for(int i = 0; i < A.getRows(); i++)
        for(int j = 0; j < A.getCols(); j++)
            result.set(i, j, A.get(i,j) + B.get(i,j));

    return result;
}