#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "timing.h"

// declare matrieces
double* mat_a;
double* mat_b;
double* mat_c;

// rows and cols of matrix
uint64_t n;

/**
 * Initializes matrix mat, such that each element equals to its
 * index. This function is for convenience, it is not required
 * to use it.
 */
void init_mat( double* mat, uint64_t rows, uint64_t cols ) {
    
    for( uint64_t i = 0; i < rows; ++i ) {
        for( uint64_t j = 0; j < cols; ++j ) {
            /*
             * matrix 4x4
             * example i = 0, cols = 4 j = 0 ... 3 = i*cols + j = 00 01 02 03
             * example i = 1, cols = 4 j = 0 ... 3 = i*cols + j = 04 05 06 07
             * example i = 2, cols = 4 j = 0 ... 3 = i*cols + j = 08 09 10 11
             * example i = 3, cols = 4 j = 0 ... 3 = i*cols + j = 12 13 14 15
             */
	        mat[i*cols + j] = i*cols + j;
	        printf("%f ", mat[i*cols + j]);
	        printf("\n");
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
	            // check mat
	            //printf("%f ", c_mat[i*c_cols + j]);
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

/**
 * simple matrix multiplication
 */
void mult_mat_simple() {
    double** a_mat = malloc(sizeof(double) * n);
    double** b_mat = malloc(sizeof(double) * n);
    double** c_mat = malloc(sizeof(double)* n);
	double res;
	for(int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				c_mat[k][i] += a_mat[i][j] * b_mat[j][i];

			}

		}
	}
}

/**
 * Prints a matrix
 */
void print_mat(double* mat, uint64_t rows, uint64_t cols)
{
		uint64_t i;
		for (i = 0; i < rows * cols; i++) {
			printf("%f ", mat[i]);
		}
		printf("\n");
}

int main( int argc, char** argv ) {

	int myrank, nprocs;
	//int master = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	//printf("%d, %d \n", myrank, nprocs);

	if (argc < 2) {
		printf("Usage: %s <n>\n", argv[0]);
		exit(1);
	}

	n = atoll(argv[1]);

	init_clock_time();

	/* Initialize */

	if (myrank == 0) {
		mat_a = malloc(sizeof(double) * (n * n));
		mat_b = malloc(sizeof(double) * (n * n));
		mat_c = malloc(sizeof(double) * (n * n));

		init_mat(mat_a, n, n);
		init_mat(mat_b, n, n);

	/*print matrix*/
		//print_mat(mat_a, n, n);
		//print_mat(mat_b, n, n);
	}

    /* Take timestamp only after all the initialization steps are over */    
    double start = get_clock_time();
	printf("1");
    
    /* Distribute */
	// master broadcasts complete matrix b to all worker processes
	MPI_Bcast(mat_b, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	printf("2");
	// master sends one row per worker process
	MPI_Scatter(mat_a, n, MPI_DOUBLE, mat_c, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	printf("3");
	/* Multiply */
    mult_mat_fast(mat_a, mat_b, mat_c, n, n, n);
	/* Collect */
	printf("4");
    MPI_Gather(mat_a, n*n, MPI_DOUBLE, mat_c, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /* Take timestamp only after the collection/gathering step is over */    
    double elapsed = get_clock_time() - start;
	printf("5");
    /* Verify */


    /* Get timing - max across all ranks */
    double elapsed_global;
    MPI_Reduce( &elapsed, &elapsed_global, 1, MPI_DOUBLE, 
                MPI_MAX, 0, MPI_COMM_WORLD );
                
    if( myrank == 0 ) {
//	    print_mat(mat_a, n, n);
//	    print_mat(mat_c, n, n);
        printf( "Elapsed time (ms): %f\n", elapsed_global );
    }

    MPI_Finalize();

	free(mat_a);
    free(mat_b);
    free(mat_c);

	return 0;
}
