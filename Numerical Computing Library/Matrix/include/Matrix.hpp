#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>

class SparseMatrix {
protected:
    int n;
    std::vector<double> values;     // non-zero values
    std::vector<int> colIndex;      // column index
    std::vector<int> rowPtr;        // row pointer

    std::vector<double> B;

public:
    SparseMatrix(int n);

    void inputCSR();   // user input
    void print();

    int size() const;
};

#endif