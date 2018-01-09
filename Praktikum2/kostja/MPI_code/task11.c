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
			mat[i * cols + j] = i * cols + j;
			printf("%f ", mat[i * cols + j]);
		}
	}
	printf("\n");
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
	/* example: mat 4x4
	 * r is intermediate stored values from mat a
	 * */
	for (uint64_t k = 0; k < a_cols; ++k) {
		for (uint64_t i = 0; i < c_rows; ++i) {
			double r = a_mat[i * a_cols + k];
			for (uint64_t j = 0; j < c_cols; ++j) {
				c_mat[i * c_cols + j] += r * b_mat[k * c_cols + j];
			}
		}
	}
}


/**
 * This function verifies the result.
 * Returns 1 if multiplication is correct, and 0 otherwise.
 */
int verify_results(double *mat, uint64_t n) {

	return 0;
}

/**
 * prints matrix
 * @param mat
 * @param size
 */
void print_mat(double *mat, int size) {
	for (int i = 0; i < size*size; i++) {
		printf("%f", mat[i]);
		if(i%size == 0)
			printf("\n");
	}
	printf("\n\n");
}


int main(int argc, char **argv) {
	int myrank, nprocs;
	uint64_t n;
	double mat_a[n * n];
	double mat_b[n * n];
	double mat_c[n * n];
	// dummys
	double mat_aa[n * n];
	double mat_cc[n * n];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if (argc < 2) {
		printf("Usage: %s <n>\n", argv[0]);
		exit(1);
	}

	n = atoll(argv[1]);

	init_clock_time();

	/* Initialize */
	if (myrank == 0) {
		init_mat(mat_a, n, n);
		init_mat(mat_b, n, n);
	}

	/* Take timestamp only after all the initialization steps are over */
	double start = get_clock_time();

	/* Distribute */
	MPI_Scatter(mat_a, n * n / nprocs, MPI_DOUBLE, mat_aa, n * n / nprocs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	printf("2\n");
	// master sends one row per worker process
	MPI_Bcast(mat_b, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	printf("3\n");

	/* Multiply */
	mult_mat_fast(mat_a, mat_b, mat_c, n, n, n);
	/* Collect */
	MPI_Gather(mat_c, n * n / nprocs, MPI_DOUBLE, mat_cc, n * n / nprocs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/* Take timestamp only after the collection/gathering step is over */
	double elapsed = get_clock_time() - start;

	/* Verify */


	/* Get timing - max across all ranks */
	double elapsed_global;
	MPI_Reduce(&elapsed, &elapsed_global, 1, MPI_DOUBLE,
	           MPI_MAX, 0, MPI_COMM_WORLD);
	if (myrank == 0) {
		print_mat(mat_cc, n);
		print_mat(mat_c, n);
		printf("Elapsed time (ms): %f\n", elapsed_global);
	}

	MPI_Finalize();

	return 0;
}
