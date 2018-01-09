#include "MandelBrot.h"
#include <sstream>
#include <iostream>

// needs the folowing parameters:
// Xmin min real part of complexnumber (best not smaller than -2)
// Xmax max real part of complexnumber (best not bigger than 2)
// Ymin min img part of complexnumber (best not smaller than -2)
// Ymax max img part of complexnumber (best not bigger than 2)
// max number of iterations
int main(int argc, char* argv[]) {
    // get the needed arguments
    int xMin, xMax, yMin, yMax, iterations, threadNum;
    try {
        xMin = std::stoi(argv[1]);
        xMax = std::stoi(argv[2]);
        yMin = std::stoi(argv[3]);
        yMax = std::stoi(argv[4]);
        iterations = std::stoi(argv[5]);
        threadNum = std::stoi(argv[6]);
    } catch (...) {
        std::cerr << 
            "Invalid program arguments please pass the following arguments: \n" <<
            "a int for min real part of used  complexnumbers \n" <<
            "a int for max real part of used  complexnumbers \n" <<
            "a int for min img part of used  complexnumbers \n" <<
            "a int for max img part of used  complexnumbers \n" <<
            "a int for max iterations to be computed \n"
            "a int for number of threads to be used \n"
            ;
        return 1; // exit program
    }

    Mandelbrot mB(xMin, xMax, yMin, yMax, iterations, threadNum);
    ppmwrite("MEINFREUND", mB.mField, iterations);

    return 0;
}
