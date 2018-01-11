#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>


/**
 * Initializes matrix mat.
 */
void init_mat( double* mat, uint64_t q, int coords[2], int dim ) {
    
}


/**
 * Cache-friendly (fast) variant of matrix multiplication. 
 * Multiplies a_mat by b_mat and stores the result in c_mat.
 * This function can be used for multiplying parts of matrices 
 * and for verifying the results in the end.
 */
void mult_mat_fast( double* a_mat, double* b_mat, double* c_mat,
                    uint64_t c_rows, uint64_t c_cols, uint64_t a_cols ) {
    
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
 * This function verifies the result for the local block.
 * Returns 1 if multiplication is correct, and 0 otherwise.
 */
int verify_results( double* mat, uint64_t q, int coords[2], int dim ) {

    return 0;
}


int main( int argc, char** argv ) {

    int world_rank, cart_rank, world_size;
    double* a_mat = NULL;
    double* b_mat = NULL;
    double* c_mat = NULL;
    MPI_Comm comm_cart = MPI_COMM_NULL;
    
    /* Initialize and get initial info */
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &world_rank );
    MPI_Comm_size( MPI_COMM_WORLD, &world_size );

    if( argc < 2 ) {
        if( world_rank == 0) {
            printf( "Usage: %s <block size>\n", argv[0] );
        }
        MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
    }

    /* Block size */
    uint64_t q = atoll( argv[1] );
    
    /* Create a cartesian communicator (subtask 2.1) */

    /* Initialize local blocks of A, B, and C (subtask 2.2) */
        
    /* Initial skewing of the blocks  (subtask 2.3) */
    
    /* Shift matrices A and B (subtask 2.4) */

    /* Perform local matrix multiply on initial data (subtask 2.5) */
       
    /* Main iteration (subtask 2.6) */
        
    /* Verify the results (subtask 2.7) */
    int global_result = 0;
    if( world_rank == 0 ) {
        printf( "Correct: %d\n", global_result );
    }
    
    
    MPI_Finalize();

    return 0;
}
