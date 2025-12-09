#include <iostream>
#include <autodiff.hpp>
#include <nonlinfunc.hpp>
          

int main()
{
    using namespace ASC_ode;

    PendulumAD pend(1.0);

    Vector<double> x(2);
    x(0) = 0.5;
    x(1) = 0.0;

    Vector<double> f(2);
    pend.evaluate(x, f);

    std::cout << "f(x) = [" << f(0) << ", " << f(1) << "]\n";

    Matrix<double> J(2,2);
    pend.evaluateDeriv(x, J);

    std::cout << "Jacobian df/dx:\n";
    for(size_t i=0;i<2;i++)
    {
        for(size_t j=0;j<2;j++)
            std::cout << J(i,j) << " ";
        std::cout << "\n";
    }

    return 0;
}
