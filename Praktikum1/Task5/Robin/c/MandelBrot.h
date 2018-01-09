#pragma once

#include "ppmwrite.h"
#include "complex.h"

class Mandelbrot {
public:
    // a field containing 800 x 800 Points
    field mField;

    // the constructor of Mandelbrot class
    Mandelbrot(int xMin, int xMax, int yMin, int yMax, int maxIterations, int threadNum);
    
    // computes the mandelbrot field
    void computeField();

    int computePoint(Complex z, Complex c);

private:
   int mXMin, mXMax, mYMin, mYMax, mMaxIterations, mThreadNum;
};

