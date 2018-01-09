#include "complex.h"
#include <iostream>
int main()
{
    Complex a(1, 1);
    Complex b(2, 2);
    Complex c(0, 0);

    c.add(a);
    c.print();

    b.mult(a);
    b.print();

    std::cout << "norm of a is = " << a.norm() << "\n";
}
