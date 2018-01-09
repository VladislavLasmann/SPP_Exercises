#include "complex.h"
#include <cmath>
#include <iostream>

Complex::Complex(double real2, double img2)
    : real(real2)
    , img(img2)
{}

void Complex::add(Complex b)
{ 
    real += b.real;
    img += b.img;
}

void Complex::mult(Complex b)
{
    double tmp = real;
    real = real * b.real - img * b.img; 
    img = (tmp * b.img) + (img * b.real);
}

double Complex::norm() {
    return std::sqrt((real * real) + (img * img));
}

void Complex::setReal(double real2) {
    real = real2;
}

void Complex::setImg(double img2) {
    img = img2;
}

void Complex::print()
{
    std::cout << "Real: " << real << "\nImg: " << img << "\n";
}
