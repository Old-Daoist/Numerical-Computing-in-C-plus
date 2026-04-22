#include "../../include/operations/Divide.hpp"
#include "../../include/operations/Multiply.hpp"
#include <stdexcept>

Matrix Divide::compute(const Matrix& A, const Matrix& B) {
    if (A.getCols() != B.getRows()) {
        throw std::invalid_argument("Matrix dimensions incompatible for division: A columns must equal B rows");
    }
    Matrix inv = B.inverse();
    return Multiply::compute(A, inv);
}