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
void init_mat(double *mat, uint64_t rows, uint64_t cols) {

	for (uint64_t i = 0; i < rows; ++i) {
		for (uint64_t j = 0; j < cols; ++j) {
			/*
			 * matrix 4x4
			 * example i = 0, cols = 4 j = 0 ... 3 = i*cols + j = 00 01 02 03
			 * example i = 1, cols = 4 j = 0 ... 3 = i*cols + j = 04 05 06 07
			 * example i = 2, cols = 4 j = 0 ... 3 = i*cols + j = 08 09 10 11
			 * example i = 3, cols = 4 j = 0 ... 3 = i*cols + j = 12 13 14 15
			 */
			mat[i * cols + j] = i * cols + j;
			//printf("%f ", mat[i * cols + j]);
		}
	}
	//printf("\n\n");
}


/**
 * Cache-friendly (fast) variant of matrix multiplication. 
 * Multiplies a_mat by b_mat and stores the result in c_mat.
 * This function can be used for multiplying parts of matrices 
 * and for verifying the results in the end.
 */
void mult_mat_fast(double *a_mat, double *b_mat, double *c_mat,
                   uint64_t c_rows, uint64_t c_cols,
                   uint64_t a_cols) {

	for (uint64_t k = 0; k < a_cols; ++k) {
		for (uint64_t i = 0; i < c_rows; ++i) {
			double r = a_mat[i * a_cols + k];
			for (uint64_t j = 0; j < c_cols; ++j) {
				c_mat[i * c_cols + j] += r * b_mat[k * c_cols + j];
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
int verify_results(double *mat, uint64_t n) {
	// for verification-check process the matrix-multiplication
	double* a_mat = (double*) malloc( sizeof(double) * n * n );
	double* b_mat = (double*) malloc( sizeof(double) * n * n );
	double* c_mat = (double*) malloc( sizeof(double) * n * n );

	init_mat(a_mat, n, n);
	init_mat(b_mat, n, n);

	mult_mat_fast(a_mat, b_mat, c_mat, n, n, n);

	// verification, checking the matrix-elements
	for(int i = 0; i < n*n; ++i){
		if( mat[i] != c_mat[i] ){
			printf("FALSE! mat[%d] = %f != matrixResult[%d] = %f\n", i, mat[ i ], i, c_mat[i] );
			free(a_mat);
			free(b_mat);
			free(c_mat);
			return 0;
		}
	}

	free(a_mat);
	free(b_mat);
	free(c_mat);

	return 1;
}

int main(int argc, char **argv) {

	int myrank, nprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	//printf("%d, %d \n", myrank, nprocs);

	if (argc < 2) {
		printf("Usage: %s <n>\n", argv[0]);
		exit(1);
	}

	uint64_t n = atoll(argv[1]);

	init_clock_time();

	// variable initialisation
	const int MASTER_RANK = 0;
	uint64_t block_size = (n * n) / nprocs;
	uint64_t row_size = n / nprocs;
	uint64_t row_start_idx = myrank * block_size;

	// allocate memory for matrices
	// why casts to double pointer?
	// How looks processors to content between MPI_init and MPI_finalize?
	// because of testing with printf("%d, %d \n", myrank, nprocs); all
	// procs get the content unless if clauses with master rank and specific
	// function only for master rank

	double *mat_a = (double *) malloc(sizeof(double) * n * n);
	double *mat_b = (double *) malloc(sizeof(double) * n * n);
	double *mat_c = mat_b;
	// partial matrix for what and why?
	double *part_mat_c = (double *) malloc(sizeof(double) * block_size);

	// check divisibility of matrix by processors (nprocs)
	if (n % nprocs != 0) {
		if (myrank == 0) printf("Matrix size not divisible by number of processors\n");
		MPI_Finalize();
		exit(-1);
	}

	/* Initialize   --------------------------------------------------------------------*/
	if (myrank == MASTER_RANK) {
		init_mat(mat_a, n, n);
		init_mat(mat_b, n, n);
	}

	/* Take timestamp only after all the initialization steps are over */
	double start = get_clock_time();

	// barrier because race condition of particular procs. master rank might
	// be slower then the other ranks.
	// but is it relevant? then scatter is a blocking function but broadcast not
	// maybe yes because broadcast is non blocking and matrix B could be uninitialised
	// when matrix b broadcasts to procs
	MPI_Barrier(MPI_COMM_WORLD);

	/* Distribute   --------------------------------------------------------------------*/
	// master broadcasts complete matrix b to all worker processes
	MPI_Bcast(mat_b, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// master sends one row per worker process
	// row_start_idx = myrank * block_size
	// receive buffer (&mat_a[row_start_idx]) have to be at the right position for each
	// processor so the received partial matrix set is disjointed on each particular processor
	//MPI_Scatter(mat_a, n, MPI_DOUBLE, mat_c, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(mat_a, block_size, MPI_DOUBLE, &mat_a[row_start_idx], block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	/* Multiply     --------------------------------------------------------------------*/
	MPI_Barrier(MPI_COMM_WORLD);
	// &mat_a[row_start_idx] - all particular processors should manipulate dijointed
	// sets of matrix a
	// row_size - n / nporcs; mistaken n with nprocs
	// part_mat_c - each proc writes to a part therefore row size should be adjust to n/nprocs
	// mult_mat_fast(mat_a, mat_b, mat_c, n, n, n);
	mult_mat_fast(&mat_a[row_start_idx], mat_b, part_mat_c, row_size, n, n);

	/* Collect      --------------------------------------------------------------------*/
	// probably dont needed because gather is a blocking com function or implicit barrier
	MPI_Barrier(MPI_COMM_WORLD);
	// part_mat_c - show line 176
	// block_size - (n*n)/ procs, mistaken with n
	// mat_c - clearly
	//MPI_Gather(mat_a, n * n, MPI_DOUBLE, mat_c, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Gather(part_mat_c, block_size, MPI_DOUBLE, mat_c, block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	/* Take timestamp only after the collection/gathering step is over */
	double elapsed = get_clock_time() - start;

	/* Verify       --------------------------------------------------------------------*/
	// probably dont needed because gather is a blocking com function or implicit barrier
	MPI_Barrier(MPI_COMM_WORLD);

	if (myrank == MASTER_RANK) {

        printf("printing result-matrix:\n");
        for(int i = 0; i < n * n; i++){
            printf("matrix_c[%d]=%f\n", i, mat_c[i]);
        }
		printf("\n\n");

		printf("==================================\nVerification...\n");
		if (verify_results(mat_c, n)) {
			printf("Multiplication worked correctly\n");
		} else {
			printf("Multiplication-results are false\n");
		}

	}

	/* Get timing - max across all ranks */
	double elapsed_global;
	MPI_Reduce(&elapsed, &elapsed_global, 1, MPI_DOUBLE,
	           MPI_MAX, 0, MPI_COMM_WORLD);

	if (myrank == 0) {
		printf("Elapsed time (ms): %f\n", elapsed_global);
	}

	free(mat_a);
	free(mat_b);
	free(part_mat_c);

	MPI_Finalize();

	return 0;
}
