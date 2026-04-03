#ifndef SPARSE_MATRIX_HPP
#define SPARSE_MATRIX_HPP

#include <vector>

class SparseMatrix {
protected:
    int n;
    std::vector<double> values;
    std::vector<int> colIndex;
    std::vector<int> rowPtr;
    std::vector<double> B;

public:
    SparseMatrix(int n);

    void inputCSR();
    int size() const;

    std::vector<double>& getB();
};

#endif