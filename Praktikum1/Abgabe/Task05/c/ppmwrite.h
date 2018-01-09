
const int width=800, height=800;     // Resolution
typedef int field[width][height];    // Type to store iteration counts

void ppmwrite(const char *fname,     // Name of PPM file
              const field iterations,// Calculated iteration counts
              int maxiter);          // Iteration count limit
