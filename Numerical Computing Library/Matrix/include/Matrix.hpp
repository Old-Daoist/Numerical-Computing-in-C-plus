#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <iostream>
#include <string>

class Matrix {
private:
    int rows, cols;
    std::vector<std::vector<double>> data;

public:
    Matrix(int r = 0, int c = 0);

    void input();
    void inputFromFile(const std::string& filename);
    void display() const;
    void saveToFile(const std::string& filename) const;

    int getRows() const;
    int getCols() const;

    double get(int i, int j) const;
    void set(int i, int j, double val);
};

#endif