#include <iostream>
#include "include/Bisection.hpp"

using namespace std;

double func(double x) {
    return x*x - 4;
}

int main() {
    int choice;
    cout << "1. Bisection\n2. Newton-Raphson\n";
    cin >> choice;

    if(choice == 1) {
        double root = Bisection::solve(func, 0, 3, 0.001, 100);
        cout << "Root: " << root;
    }

    return 0;
}