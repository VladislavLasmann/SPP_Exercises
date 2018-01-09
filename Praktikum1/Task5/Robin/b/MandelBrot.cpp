#include "MandelBrot.h"
#include "complex.h"

Mandelbrot::Mandelbrot(int xMin, int xMax, int yMin, int yMax, int maxIterations)
    : mXMin(xMin),
      mXMax(xMax),
      mYMin(yMin),
      mYMax(xMax),
      mMaxIterations(maxIterations) {
          computeField();
}

void Mandelbrot::computeField() {
    const double incX = (mXMax - mXMin) / static_cast<double>(width);
    const double incY = (mYMax - mYMin) / static_cast<double>(height);
    Complex c(mXMin, mYMax);
    Complex stepX(incX, 0);
    Complex stepY(0, -incY);
    
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Complex z(0,0);
            mField[i][j] = computePoint(z, c);
            c.add(stepX);
        }
        c.setReal(mXMin);
        c.add(stepY);
    }
}

// gives back the number of iterations for choosen Point
int  Mandelbrot::computePoint(Complex z, Complex c) {
    int i = 0;
    for (i = 0; i <= mMaxIterations && z.norm() < 16; i++) {
        z.mult(z);
        z.add(c);
    }
    return i;
}