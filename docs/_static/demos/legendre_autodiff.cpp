#include <iostream>
#include "autodiff.hpp"
#include <fstream>
#include <vector>


using namespace ASC_ode;

template <typename T>
std::vector<T> LegendrePolynomials(int n, T x) {
    std::vector<T> P(n + 1);
    if (n >= 0) P[0] = T(1);
    if (n >= 1) P[1] = x;

    for (int k = 2; k <= n; ++k) {
        P[k] = ((T(2*k - 1) * x * P[k-1]) - T(k - 1) * P[k-2]) / T(k);
    }

    return P;
}


int main()
{
 std::ofstream out("Legendre/legendre.csv");
    out << "x,P0,dP0_dx,P1,dP1_dx,P2,dP2_dx,P3,dP3_dx,P4,dP4_dx,P5,dP5_dx\n";

    for (double xv = -1; xv <= 1.00001; xv += 0.01) {
        AutoDiff<1> x = Variable<0>(xv);

        out << xv;
        auto P = LegendrePolynomials(5, x);

        for (int n = 0; n <= 5; n++) {
            out << "," << P[n].value()
                << "," << P[n].deriv()[0];
        }
        out << "\n";
    }
    out.close();
    return 0;
}