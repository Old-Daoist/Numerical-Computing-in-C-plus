#include "../../include/operations/Divide.hpp"
#include "../../include/operations/Multiply.hpp"
#include <stdexcept>

Matrix Divide::compute(const Matrix& A, const Matrix& B) {
    if(B.getRows() != 2 || B.getCols() != 2)
        throw std::invalid_argument("Only 2x2 supported");

    double a = B.get(0,0);
    double b = B.get(0,1);
    double c = B.get(1,0);
    double d = B.get(1,1);

    double det = a*d - b*c;

    if(det == 0)
        throw std::runtime_error("Not invertible");

    Matrix inv(2,2);
    inv.set(0,0, d/det);
    inv.set(0,1,-b/det);
    inv.set(1,0,-c/det);
    inv.set(1,1, a/det);

    return Multiply::compute(A, inv);
}