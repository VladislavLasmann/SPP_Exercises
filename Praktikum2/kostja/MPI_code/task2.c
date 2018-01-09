#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>


/**
 * Initializes matrix mat.
 */
void init_mat(double *mat, uint64_t q, int coords[2], int dim) {
	uint64_t n = q * sqrt(dim);

	uint64_t I = coords[0] * n;
	uint64_t J = coords[1] * n;
	//printf("Matrix size: %lu\n", sizeof(mat));
	//printf("i = %llu , j = %llu\n", I, J);

	//printf("n = %llu\n", n);
	uint64_t start_idx = I * q + J;
	//printf("start_idx: %llu\n", start_idx);
	//double *mat_start = mat + start_idx;
	//printf("%p\n", mat_start);

	//for (uint64_t i = 0; i < q; ++i) {
		for (uint64_t j = 0; j < n; ++j) {
			//printf("init %llu\n", i*cols +j);
			mat[j] = start_idx + j;
		//	printf("mat[%llu] = %f     ", j, mat[j]);
		}
		//printf("coords: %d %d, next rank ->\n\n", coords[0], coords[1]);
	//}

}


/**
 * Cache-friendly (fast) variant of matrix multiplication. 
 * Multiplies a_mat by b_mat and stores the result in c_mat.
 * This function can be used for multiplying parts of matrices 
 * and for verifying the results in the end.
 */
void mult_mat_fast(double *a_mat, double *b_mat, double *c_mat,
                   uint64_t c_rows, uint64_t c_cols, uint64_t a_cols) {

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
 * This function verifies the result for the local block.
 * Returns 1 if multiplication is correct, and 0 otherwise.
 */
int verify_results(double *mat, uint64_t q, int coords[2], int dim) {

	return 0;
}


int main(int argc, char **argv) {

	int world_rank, cart_rank, world_size;
	double *a_mat = NULL;
	double *b_mat = NULL;
	double *c_mat = NULL;
	MPI_Comm comm_cart = MPI_COMM_NULL;

	/* Initialize and get initial info */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (argc < 2) {
		if (world_rank == 0) {
			printf("Usage: %s <block size>\n", argv[0]);
		}
		MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
	}

	const int MASTER = 0;

	/* Block size */
	uint64_t q = atoll(argv[1]);

	// q should be < number of processes (world_size)
	//if (q > world_size) {
	//	if (world_rank == MASTER) printf("Matrix size not divisible by number of processors\n");
	//	MPI_Abort( MPI_COMM_WORLD, EXIT_FAILURE );
	//}



	/* Create a cartesian communicator (subtask 2.1) */
	// number of dimensions of cartesian grid
	int ndims = 2;

	// integer array of size ndims specifying the number of processes in each dimension
	int dims[2];
	// q = n / √p but what is n and p in this case
	dims[0] = q;
	// test it on cluster previous value q
	dims[1] = q;

	// logical array of size ndims specifying whether the grid is periodic (true) or not (false) in each dimension
	int periods[ndims];
	// matrix is periodic
	periods[0] = 1;
	periods[1] = 1;

	/* ranking may be reordered (true) or not (false) / maybe true
	 * If reorder = false then the rank of each process in the new
	 * group is identical to its rank in the old group. Otherwise,
	 * the function may reorder the processes (possibly so as to
	 * choose a good embedding of the virtual topology onto the physical machine).*/
	int reorder = 1;

	// Makes a new communicator to which topology information has been attached
	MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &comm_cart);
	// probably cart_rank
	//int rank;

	// Determines process rank in communicator given Cartesian location
	MPI_Cart_rank(comm_cart, dims, &cart_rank);
	//printf("cart_rank: %d\n", cart_rank);

	int maxdims = ndims;
	int coords[2];
	// Determines process coords in cartesian topology given rank in group
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Cart_coords(comm_cart, world_rank, maxdims, coords);
	MPI_Barrier(MPI_COMM_WORLD);
	printf("Rank %d coordinates are %d %d\n", world_rank, coords[0], coords[1]);

	/* Initialize local blocks of A, B, and C (subtask 2.2) */
	a_mat = malloc(sizeof(double) * q * q);
	b_mat = malloc(sizeof(double) * q * q);
	c_mat = malloc(sizeof(double) * q * q);
	MPI_Barrier(MPI_COMM_WORLD);

	init_mat(a_mat, q, coords, world_size);
	init_mat(b_mat, q, coords, world_size);

	//if (world_rank == MASTER) {
		for (int i = 0; i < q * q; i++) {
			printf("mat[%d] = %f \n", i, a_mat[i]);
		}
	//}

	/* Initial skewing of the blocks  (subtask 2.3) */
	int direction = 0;
	int displ = 1;
	int source;
	int dest;

	MPI_Cart_shift(comm_cart, direction, displ, source, dest);

	/* Shift matrices A and B (subtask 2.4) */

	/* Perform local matrix multiply on initial data (subtask 2.5) */

	/* Main iteration (subtask 2.6) */

	/* Verify the results (subtask 2.7) */
	int global_result = 0;
	if (world_rank == 0) {
		printf("Correct: %d\n", global_result);
	}
	MPI_Comm_free(comm_cart);
	MPI_Barrier(MPI_COMM_WORLD);
	free(a_mat);
	free(b_mat);
	free(c_mat);

	MPI_Finalize();

	return 0;
}
