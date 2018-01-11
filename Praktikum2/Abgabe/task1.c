/**
 * Gruppe: 122
 * Konstantin Müller (2327697) 
 * Robin Ferrari 	 (2585277) 
 * Vladislav Lasmann (2593078)
 */
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
            //printf("init %llu\n", i*cols +j);
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
    // for verification-check process the matrix-multiplication 
    double* matrixA = (double*) calloc(n*n, sizeof(double));
    double* matrixB = (double*) calloc(n*n, sizeof(double));
    double* matrixResult = (double*) calloc(n*n, sizeof(double));
    init_mat(matrixA, n, n);
    init_mat(matrixB, n, n);
    mult_mat_fast(matrixA, matrixB, matrixResult, n, n, n);

    // verification, checking the matrix-elements
    for(uint64_t i = 0; i < n*n; ++i){
        if( mat[i] != matrixResult[i] ){
            //printf("FALSE! mat[%d] = %f != matrixResult[%d] = %f\n", i, mat[ i ], i, matrixResult[i] );
            free(matrixA);
            free(matrixB);
            free(matrixResult);
            return 0;
        }
    }

    free(matrixA);
    free(matrixB);
    free(matrixResult);

    return 1;
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

    /* Declaring variables */
    const int MASTER_RANK = 0;          
    int myBlockCount = (n * n) / nprocs;                
    int myRowAmount = n / nprocs;                       
    int myRowStartIndex = myrank * myBlockCount;

    /* Allocate memory for matrices & part-matrix*/
    double* matrixA = (double*) calloc(n*n, sizeof(double));
    double* matrixB = (double*) calloc(n*n, sizeof(double));
    double* partMatrixC = (double*) calloc(myBlockCount, sizeof(double));
    //double* matrixA = (double*) malloc( sizeof(double) * n * n);
    //double* matrixB = (double*) malloc( sizeof(double) * n * n);
    double* matrixC = matrixB;
    //double* partMatrixC = (double*) malloc( sizeof(double) * myBlockCount );


    // check if matrix is divisible by processors
    if( n%nprocs != 0 ){
        if( myrank == MASTER_RANK ){
            printf("Size of quadratic matrix with length %d is not divisible by %d processors\n", (int) n, nprocs);
        }
        MPI_Finalize();
        return -1;
    }

    /* Initialize */
    if( myrank == MASTER_RANK ){
        //printf("initializing...\nnprocs:%d, myBlockCount:%d, myRowAmount:%d, myRowStartIndex:%d\n", nprocs, myBlockCount, myRowAmount, myRowStartIndex);
        init_mat(matrixA, n, n);
        init_mat(matrixB, n, n);
    }


    /* Take timestamp only after all the initialization steps are over */    
    double start = get_clock_time();

    /* Distribute */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(matrixB, n*n, MPI_DOUBLE, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Scatter(matrixA, myBlockCount, MPI_DOUBLE, &matrixA[myRowStartIndex], myBlockCount, MPI_DOUBLE, MASTER_RANK, MPI_COMM_WORLD);


    /* Multiply */
    MPI_Barrier(MPI_COMM_WORLD);
    mult_mat_fast(&matrixA[myRowStartIndex], matrixB, partMatrixC, myRowAmount, n, n);

    /* Collect */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(partMatrixC, myBlockCount, MPI_DOUBLE, matrixC, myBlockCount, MPI_DOUBLE, MASTER_RANK, MPI_COMM_WORLD);
    

    /* Take timestamp only after the collection/gathering step is over */    
    double elapsed = get_clock_time() - start;

    /* Verify */
    MPI_Barrier(MPI_COMM_WORLD);
    if( myrank == MASTER_RANK ){
        /*
        printf("printing result-matrix:\n");
        for(int i = 0; i < n * n; i++){
            printf("matrixC[%d]=%f\n", i, matrixC[i]);
        }
        */
        //printf("==================================\nVerification...\n");
        if( verify_results(matrixC, n) ){
            printf("Multiplication worked correctly\n");
        }
        else{
            printf("Multiplication-results are false\n");
        }
    }

    /* Get timing - max across all ranks */
    double elapsed_global;
    MPI_Reduce( &elapsed, &elapsed_global, 1, MPI_DOUBLE, 
                MPI_MAX, 0, MPI_COMM_WORLD );
                
    if( myrank == 0 ) {
        printf( "Elapsed time (ms): %f\n", elapsed_global );
    }

    free(matrixA);
    free(matrixB);
    free(partMatrixC);

    MPI_Finalize();

    return 0;
}
