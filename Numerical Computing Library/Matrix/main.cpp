#include <iostream>
#include <vector>

#include "Matrix.hpp"
#include "operations/Add.hpp"
#include "operations/Subtract.hpp"
#include "operations/Multiply.hpp"
#include "operations/Divide.hpp"

#include "solvers/SolveLinearEquation.hpp"
#include "solvers/LUDecomposition.hpp"

#include "numerical/Interpolation.hpp"
#include "numerical/Eigen.hpp"
#include "numerical/EigenPower.hpp"
// ================= MATRIX INPUT =================
void loadMatrix(Matrix &M) {
    int choice;
    std::cout << "\n1. Input manually\n2. Input from file\nChoice: ";
    std::cin >> choice;

    if(choice == 1) {
        int r, c;
        std::cout << "Enter rows and columns: ";
        std::cin >> r >> c;

        M = Matrix(r, c);
        M.input();
    } else {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;
        M.inputFromFile(filename);
    }
}

// ================= OUTPUT =================
void outputResult(const Matrix& result) {
    int choice;

    std::cout << "\nOutput options:\n";
    std::cout << "1. Display on terminal\n";
    std::cout << "2. Save to file\n";
    std::cout << "3. Both\n";
    std::cout << "Choice: ";
    std::cin >> choice;

    if(choice == 1) {
        result.display();
    }
    else if(choice == 2) {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;
        result.saveToFile(filename);
        std::cout << "Saved to file.\n";
    }
    else if(choice == 3) {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;

        result.display();
        result.saveToFile(filename);

        std::cout << "Also saved to file.\n";
    }
    else {
        std::cout << "Invalid choice\n";
    }
}

// ================= VECTOR INPUT =================
std::vector<double> inputVector(int n) {
    std::vector<double> b(n);
    std::cout << "Enter RHS vector:\n";
    for(int i = 0; i < n; i++)
        std::cin >> b[i];
    return b;
}

void printVector(const std::vector<double>& x) {
    std::cout << "Solution:\n";
    for(double v : x)
        std::cout << v << " ";
    std::cout << std::endl;
}

// ================= VALIDATION =================
bool isSquare(const Matrix& M) {
    return M.getRows() == M.getCols();
}

// ================= MAIN =================
int main() {
    Matrix A, B;
    int choice;

    while(true) {
        std::cout << "\n==== MATRIX MENU ====\n";
        std::cout << "1. Load Matrix A\n";
        std::cout << "2. Load Matrix B\n";
        std::cout << "3. Add\n";
        std::cout << "4. Subtract\n";
        std::cout << "5. Multiply\n";
        std::cout << "6. Divide (2x2 only)\n";
        std::cout << "7. Gauss Elimination\n";
        std::cout << "8. Gauss Jacobi\n";
        std::cout << "9. Gauss Seidel\n";
        std::cout << "10. LU Decomposition Solve\n";
        std::cout << "11. Lagrange Interpolation\n";
        std::cout << "12. Gershgorin Eigenvalue Estimate\n";
        std::cout << "13. Power Method (Eigenvalue & Eigenvector)\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        try {
            switch(choice) {

                case 1:
                    loadMatrix(A);
                    break;

                case 2:
                    loadMatrix(B);
                    break;

                case 3: {
                    Matrix result = Add::compute(A,B);
                    outputResult(result);
                    break;
                }

                case 4: {
                    Matrix result = Subtract::compute(A,B);
                    outputResult(result);
                    break;
                }

                case 5: {
                    Matrix result = Multiply::compute(A,B);
                    outputResult(result);
                    break;
                }

                case 6: {
                    Matrix result = Divide::compute(A,B);
                    outputResult(result);
                    break;
                }

                case 7: {
                    if(!isSquare(A)) {
                        std::cout << "Matrix must be square!\n";
                        break;
                    }
                    std::vector<double> b = inputVector(A.getRows());
                    auto x = GaussElimination::solve(A, b);
                    printVector(x);
                    break;
                }

                case 8: {
                    if(!isSquare(A)) {
                        std::cout << "Matrix must be square!\n";
                        break;
                    }
                    std::vector<double> b = inputVector(A.getRows());
                    auto x = GaussJacobi::solve(A, b);
                    printVector(x);
                    break;
                }

                case 9: {
                    if(!isSquare(A)) {
                        std::cout << "Matrix must be square!\n";
                        break;
                    }
                    std::vector<double> b = inputVector(A.getRows());
                    auto x = GaussSeidel::solve(A, b);
                    printVector(x);
                    break;
                }
                case 10: {
                    if(A.getRows() != A.getCols()) {
                        std::cout << "Matrix must be square!\n";
                        break;
                   }

                   std::vector<double> b = inputVector(A.getRows());
                   auto x = LU::solve(A, b);
                   printVector(x);
                   break;
               }
               case 11: {
                  int n;
                    std::cout << "Enter number of data points: ";
                    std::cin >> n;

                    std::vector<double> x(n), y(n);

                    std::cout << "Enter x values:\n";
                    for(int i = 0; i < n; i++) std::cin >> x[i];

                    std::cout << "Enter y values:\n";
                    for(int i = 0; i < n; i++) std::cin >> y[i];
 
                    double value;
                    std::cout << "Enter value to interpolate: ";
                    std::cin >> value;

                    double result = Interpolation::lagrange(x, y, value);

                    std::cout << "Interpolated value = " << result << "\n";
                    break;
          }

                    case 12: {
                     if(A.getRows() != A.getCols()) {
                       std::cout << "Matrix must be square!\n";
                        break;
                        }

                       Eigen::gershgorin(A);
                         break;
           }
                    case 13: {
                        if(A.getRows() != A.getCols()) {
                            std::cout << "Matrix must be square!\n";
                         break;
                       }

                       std::vector<double> eigenvector;
                       double eigenvalue = EigenPower::powerMethod(A, eigenvector);

                        std::cout << "Eigenvalue: " << eigenvalue << "\n";
                         std::cout << "Eigenvector:\n";

                        for(double v : eigenvector)
                        std::cout << v << " ";
                        std::cout << "\n";

                        break;
            }
           

                case 0:
                    return 0;

                default:
                    std::cout << "Invalid choice\n";
            }

        } catch(const std::exception &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}