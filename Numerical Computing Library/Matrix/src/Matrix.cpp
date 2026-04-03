#include "../include/Matrix.hpp"
#include <iostream>

SparseMatrix::SparseMatrix(int n) : n(n) {
    B.resize(n);
    rowPtr.resize(n+1);
}

void SparseMatrix::inputCSR() {
    int nnz;
    std::cout << "Enter number of non-zero elements: ";
    std::cin >> nnz;

    values.resize(nnz);
    colIndex.resize(nnz);

    std::cout << "Enter values:\n";
    for(int i=0;i<nnz;i++) std::cin >> values[i];

    std::cout << "Enter column indices:\n";
    for(int i=0;i<nnz;i++) std::cin >> colIndex[i];

    std::cout << "Enter row pointers:\n";
    for(int i=0;i<=n;i++) std::cin >> rowPtr[i];

    std::cout << "Enter B vector:\n";
    for(int i=0;i<n;i++) std::cin >> B[i];
}

int SparseMatrix::size() const { return n; }