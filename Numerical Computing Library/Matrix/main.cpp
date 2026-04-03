#include <iostream>
#include "include/GaussSeidel.hpp"
#include "include/GaussJacobi.hpp"

int main() {
    int n, choice;

    std::cout << "Enter matrix size: ";
    std::cin >> n;

    std::cout << "1. Gauss-Seidel\n2. Gauss-Jacobi\nChoice: ";
    std::cin >> choice;

    SolveLinearEquation* solver;

    if(choice == 1)
        solver = new GaussSeidel(n);
    else
        solver = new GaussJacobi(n);

    solver->inputCSR();

    if(!solver->isDiagonallyDominant())
        std::cout << "Warning: Matrix is not diagonally dominant\n";

    auto result = solver->solve();

    std::cout << "\nSolution:\n";
    for(double x : result)
        std::cout << x << " ";

    delete solver;
    return 0;
}