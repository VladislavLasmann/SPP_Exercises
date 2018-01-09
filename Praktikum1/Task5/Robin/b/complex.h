#pragma once

class Complex{
public:
    Complex(double real, double img);

    void add(Complex b);
    void mult(Complex b);
    double norm();

    void setReal(double real);
    void setImg(double img);
    void print();

private:
    double real, img;
};

