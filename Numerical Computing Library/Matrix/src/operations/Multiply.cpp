#include "../../include/operations/Multiply.hpp"
#include <stdexcept>

Matrix Multiply::compute(const Matrix& A, const Matrix& B) {
    if(A.getCols() != B.getRows())
        throw std::invalid_argument("Invalid dimensions");

    Matrix result(A.getRows(), B.getCols());

    for(int i = 0; i < A.getRows(); i++) {
        for(int j = 0; j < B.getCols(); j++) {
            double sum = 0;
            for(int k = 0; k < A.getCols(); k++) {
                sum += A.get(i,k) * B.get(k,j);
            }
            result.set(i, j, sum);
        }
    }

    return result;
}