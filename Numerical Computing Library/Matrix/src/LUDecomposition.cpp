#include "../include/LUDecomposition.hpp"

LUDecomposition::LUDecomposition(int n) : Matrix(n) {}

void LUDecomposition::decompose(std::vector<std::vector<double>>& L,
                               std::vector<std::vector<double>>& U) {

    L.assign(n, std::vector<double>(n,0));
    U.assign(n, std::vector<double>(n,0));

    for(int i=0;i<n;i++) {
        for(int k=i;k<n;k++) {
            double sum=0;
            for(int j=0;j<i;j++)
                sum+=L[i][j]*U[j][k];
            U[i][k]=A[i][k]-sum;
        }

        for(int k=i;k<n;k++) {
            if(i==k) L[i][i]=1;
            else {
                double sum=0;
                for(int j=0;j<i;j++)
                    sum+=L[k][j]*U[j][i];
                L[k][i]=(A[k][i]-sum)/U[i][i];
            }
        }
    }
}