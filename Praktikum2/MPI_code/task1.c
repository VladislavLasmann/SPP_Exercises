#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "timing.h"

/**
 * Initializes matrix mat, such that each element equals to its
 * index. This function is for convenience, it is not required
 * to use it.
 */
void init_mat( double* mat, uint64_t rows, uint64_t cols ) {
    
    for( uint64_t i = 0; i < rows; ++i ) {
        for( uint64_t j = 0; j < cols; ++j ) {
            mat[i*cols + j] = i*cols + j;
        }
    }
} 


/**
 * Cache-friendly (fast) variant of matrix multiplication. 
 * Multiplies a_mat by b_mat and stores the result in c_mat.
 * This function can be used for multiplying parts of matrices 
 * and for verifying the results in the end.
 */
void mult_mat_fast( double* a_mat, double* b_mat, double* c_mat,
                    uint64_t c_rows, uint64_t c_cols, 
                    uint64_t a_cols ) {
    
    for( uint64_t k = 0; k < a_cols; ++k ) {
        for( uint64_t i = 0; i < c_rows; ++i ) {  
            double r = a_mat[i*a_cols + k];
            for( uint64_t j = 0; j < c_cols; ++j ) {
                c_mat[i*c_cols + j] += r * b_mat[k*c_cols + j];
            }
        }
    }
}


/**
 * This function verifies the result.
 * Returns 1 if multiplication is correct, and 0 otherwise.
 */
int verify_results( double* mat, uint64_t n ) {

    return 0;
}


int main( int argc, char** argv ) {

    int myrank, nprocs;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );
    MPI_Comm_size( MPI_COMM_WORLD, &nprocs );

    if( argc < 2 ) {
        printf( "Usage: %s <n>\n", argv[0] );
        exit( 1 );
    }

    uint64_t n = atoll( argv[1] );
    
    init_clock_time();

    /* Initialize */

    
    /* Take timestamp only after all the initialization steps are over */    
    double start = get_clock_time();

    /* Distribute */

    /* Multiply */

    /* Collect */


    /* Take timestamp only after the collection/gathering step is over */    
    double elapsed = get_clock_time() - start;

    /* Verify */


    /* Get timing - max across all ranks */
    double elapsed_global;
    MPI_Reduce( &elapsed, &elapsed_global, 1, MPI_DOUBLE, 
                MPI_MAX, 0, MPI_COMM_WORLD );
                
    if( myrank == 0 ) {
        printf( "Elapsed time (ms): %f\n", elapsed_global );
    }

    MPI_Finalize();

    return 0;
}
