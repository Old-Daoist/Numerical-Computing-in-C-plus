#include "../include/EigenValue.hpp"

EigenValue::EigenValue(int n) : Matrix(n) {}

double EigenValue::powerMethod() {
    std::vector<double> X(n,1), Y(n);
    double lambda=0;

    for(int it=0;it<100;it++) {
        for(int i=0;i<n;i++) {
            Y[i]=0;
            for(int j=0;j<n;j++)
                Y[i]+=A[i][j]*X[j];
        }

        lambda=Y[0];

        for(int i=0;i<n;i++)
            X[i]=Y[i]/lambda;
    }
    return lambda;
}