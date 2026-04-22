#ifndef QR_HPP
#define QR_HPP

#include "Matrix.hpp"
#include <vector>

// QR decomposition for m×n matrices (m >= n)
// Decomposes A into A = Q*R where:
//   Q is m×n with orthogonal columns (Q^T*Q = I_n)
//   R is n×n upper triangular
// Used for solving least-squares problems and rectangular systems
class QR {
public:
    struct Decomposition {
        Matrix Q;  // m×n orthogonal columns
        Matrix R;  // n×n upper triangular
    };
    
    // Compute QR decomposition using Householder reflections
    // Throws std::runtime_error if m < n or matrix is rank-deficient
    static Decomposition decompose(const Matrix& A);
    
    // Solve A*x = b using QR decomposition
    // For square systems: exact solution
    // For overdetermined systems (m > n): least-squares solution
    static std::vector<double> solve(const Matrix& A, const std::vector<double>& b);
};

#endif