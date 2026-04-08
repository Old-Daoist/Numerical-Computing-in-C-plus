#include "../../include/numerical/Eigen.hpp"
#include <iostream>
#include <cmath>

void Eigen::gershgorin(const Matrix& A) {
    int n = A.getRows();

    std::cout << "\nGershgorin Circles:\n";

    for(int i = 0; i < n; i++) {
        double center = A.get(i,i);
        double radius = 0;

        for(int j = 0; j < n; j++) {
            if(i != j)
                radius += std::abs(A.get(i,j));
        }

        std::cout << "Circle " << i+1 << ": Center = "
                  << center << ", Radius = " << radius << "\n";

        std::cout << "Range: [" << center - radius
                  << ", " << center + radius << "]\n\n";
    }
}