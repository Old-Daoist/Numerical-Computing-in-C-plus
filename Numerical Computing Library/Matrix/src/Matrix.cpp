#include "../include/Matrix.hpp"
#include <fstream>
#include <stdexcept>

Matrix::Matrix(int r, int c) : rows(r), cols(c) {
    data.resize(r, std::vector<double>(c, 0));
}

void Matrix::input() {
    std::cout << "Enter elements row-wise:\n";
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            std::cin >> data[i][j];
}

void Matrix::inputFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if(!file)
        throw std::runtime_error("Cannot open file");

    file >> rows >> cols;

    data.assign(rows, std::vector<double>(cols));

    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++)
            file >> data[i][j];

    file.close();
}

void Matrix::display() const {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++)
            std::cout << data[i][j] << " ";
        std::cout << std::endl;
    }
}

void Matrix::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);

    if(!file)
        throw std::runtime_error("Cannot open file for writing");

    file << rows << " " << cols << "\n";

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            file << data[i][j] << " ";
        }
        file << "\n";
    }

    file.close();
}

int Matrix::getRows() const { return rows; }
int Matrix::getCols() const { return cols; }

double Matrix::get(int i, int j) const { return data[i][j]; }

void Matrix::set(int i, int j, double val) {
    data[i][j] = val;
}