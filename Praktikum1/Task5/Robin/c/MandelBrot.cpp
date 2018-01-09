#include "MandelBrot.h"
#include "complex.h"
#include <iostream>
#include <omp.h>

Mandelbrot::Mandelbrot(int xMin, int xMax, int yMin, int yMax, int maxIterations, int threadNum)
    : mXMin(xMin),
      mXMax(xMax),
      mYMin(yMin),
      mYMax(xMax),
      mMaxIterations(maxIterations),
      mThreadNum(threadNum) {
          computeField();
}

void Mandelbrot::computeField() {
    // here we claculate the steps we need to make in the coordinate system so we map x/y min/max to
    // a resolution of width and height (from ppmwrite.h)
    const double incX = (mXMax - mXMin) / static_cast<double>(width); // resolution in pixels
    const double incY = (mYMax - mYMin) / static_cast<double>(height);

    // now lets get the time
    double startTime = omp_get_wtime();

    // now we can calculate the needed iterations for each entry in field[width][height]
    #pragma omp parallel for num_threads(mThreadNum)
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            Complex z(0,0);
            Complex c(mXMin + j*incX, mYMax - i*incY);
            mField[i][j] = computePoint(z, c);
        }
    }

    // print the time
    std::cout << "Parallel section run with " << mThreadNum << " threads in " 
        << omp_get_wtime() - startTime << " second \n";
}

// gives back the number of iterations for choosen Point
// stops if |z| >= 16 or the max iterations is reached
// for n = 0 Zn = 0 + 0i
// Zn+1 = Zn*Zn + c
int  Mandelbrot::computePoint(Complex z, Complex c) {
    int i = 0;
    for (i = 0; i <= mMaxIterations && z.norm() < 16; i++) {
        z.mult(z);
        z.add(c);
    }
    return i;
}

    


