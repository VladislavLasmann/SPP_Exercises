#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdint.h>


/**
 * Initializes matrix mat.
 * dim is the global matrix length
 * 
 */
void init_mat( double* mat, uint64_t q, int coords[2], int dim ) {
    // the first value in the global matrix (may not be the first value of the given block matrix)
    int globalMatrixLineValue = coords[0] * q * dim;

    for(uint64_t i = 0; i < q; i++){
        // the offset to the line value -> first value in the given block matrix
        int offsetToLineValue = coords[1] * q;
        for(uint64_t j = 0; j < q; j++){
            // the values in the rows are just basic sums 
            mat[i * q + j] = (double) (globalMatrixLineValue + offsetToLineValue + j);
            //printf("init_mat: coords:(%d, %d), globalMatrixLineValue: %d, offsetToLineValue: %d, q: %d,  block-matrix[%d]=%f\n", coords[0], coords[1], globalMatrixLineValue, offsetToLineValue, q, i * q + j, mat[i * q + j]);
            //printf("init_mat: coords:(%d, %d), block-matrix[%d]=%f\n", coords[0], coords[1], i * q + j, mat[i * q + j]);
                        
        }
        // update global matrix line value (adding the global matrix length to it)
        globalMatrixLineValue += dim;
    }
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
 * Adds an given matrix to the result matrix
 */
void add_toMat( double* result_mat, double* mat, uint64_t rows, uint64_t cols){
    for(uint64_t i = 0; i < (rows*cols); i++){
        result_mat[i] += mat[i];
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
    /* Process size*/
    double p = (double) world_size;
    /* squareroot of p */
    double sqrtP = sqrt(p);
    /* Dimension size */
    int DIMENSION_SIZE = 2;
    /* Block-matrix size*/
    int BLOCK_MATRIX_SIZE = q * q;
    /* Total matrix length with q = n / √p */
    int MATRIX_LENGTH = q * sqrtP;
    //printf("matrixlength: %d\n", MATRIX_LENGTH);

    /* Create a cartesian communicator (subtask 2.1) */
    int processesInDimension[DIMENSION_SIZE], periods[DIMENSION_SIZE], reorder;
    // specifying the number of precesses in each dimension
    processesInDimension[0] = sqrtP;
    processesInDimension[1] = sqrtP;
    // the communicator-struct is periodic in both dimensions
    periods[0] = 1; 
    periods[1] = 1;
    // the rank might be reordered
    reorder = 1;
    // Create cartesian communicator and save rank of each process
    MPI_Cart_create(MPI_COMM_WORLD, DIMENSION_SIZE, processesInDimension, periods, reorder, &comm_cart);
    MPI_Comm_rank(comm_cart, &cart_rank);


    /* Initialize local blocks of A, B, and C (subtask 2.2) */
    // get processes coordinates
    int processCartCoords[DIMENSION_SIZE];
    MPI_Cart_coords(comm_cart, cart_rank, DIMENSION_SIZE, processCartCoords);
    // Allocate and initialize
    a_mat = (double*) malloc( sizeof(double) * BLOCK_MATRIX_SIZE);
    b_mat = (double*) malloc( sizeof(double) * BLOCK_MATRIX_SIZE);
    c_mat = (double*) malloc( sizeof(double) * BLOCK_MATRIX_SIZE);
    // 
    init_mat(a_mat, q, processCartCoords, MATRIX_LENGTH);
    init_mat(b_mat, q, processCartCoords, MATRIX_LENGTH);
    /* 
    // INIT WORKS CORRECTLY
    printf("matrixA block after init:\n");
    for(uint64_t i = 0; i < BLOCK_MATRIX_SIZE; i++){
        printf("processCoord:(%d, %d) matrix[%d]=%f\n", processCartCoords[0], processCartCoords[1], i, a_mat[i]);
    }
    */
    /* Initial skewing of the blocks  (subtask 2.3) */
    int rankSourceShiftMatrixA, rankSourceShiftMatrixB;
    int rankDestinationShiftMatrixA, rankDestinationShiftMatrixB;
    MPI_Status statusInitShiftA, statusInitShiftB;
    // Get ranks for shifting matrixA-blocks with following rule: blocks in row i of global matrixA will be shifted i times to the left
    MPI_Cart_shift(comm_cart, 1, (-1) * processCartCoords[0], &rankSourceShiftMatrixA, &rankDestinationShiftMatrixA);
    // Get ranks for shifting matrixB-blocks with following rule: blocks in column i of global matrixB will be shifted i times up
    MPI_Cart_shift(comm_cart, 0, (-1) * processCartCoords[1], &rankSourceShiftMatrixB, &rankDestinationShiftMatrixB); 
    
    // Shifting fun.. SendRecieveTags: matrixA:1, matrixB:2
    /*
    MPI_Barrier(comm_cart);
    printf("before sendrecv matrix A \n");
    int coordsSourceShiftA[DIMENSION_SIZE], coordsDestinationShiftA[DIMENSION_SIZE];
    MPI_Cart_coords(comm_cart, rankSourceShiftMatrixA, DIMENSION_SIZE, coordsSourceShiftA);
    MPI_Cart_coords(comm_cart, rankDestinationShiftMatrixA, DIMENSION_SIZE, coordsDestinationShiftA);
    printf("myCoords: (%d, %d), sourceCoords(%d, %d), destinationCoords: (%d, %d)\n", 
    processCartCoords[0], processCartCoords[1], coordsSourceShiftA[0], coordsSourceShiftA[1], coordsDestinationShiftA[0], coordsDestinationShiftA[1]);
    MPI_Barrier(comm_cart);
    */
    MPI_Barrier(comm_cart);
    MPI_Sendrecv_replace(a_mat, BLOCK_MATRIX_SIZE, MPI_DOUBLE, rankDestinationShiftMatrixA, 1, rankSourceShiftMatrixA, 1, comm_cart, &statusInitShiftA);
    /*
    printf("before sendrecv matrix B \n");
    int coordsSourceShiftB[DIMENSION_SIZE], coordsDestinationShiftB[DIMENSION_SIZE];
    MPI_Cart_coords(comm_cart, rankSourceShiftMatrixB, DIMENSION_SIZE, coordsSourceShiftB);
    MPI_Cart_coords(comm_cart, rankDestinationShiftMatrixB, DIMENSION_SIZE, coordsDestinationShiftB);
    printf("myCoords: (%d, %d), sourceCoords(%d, %d), destinationCoords: (%d, %d)\n", 
    processCartCoords[0], processCartCoords[1], coordsSourceShiftB[0], coordsSourceShiftB[1], coordsDestinationShiftB[0], coordsDestinationShiftB[1]);
    */
    MPI_Barrier(comm_cart);
    MPI_Sendrecv_replace(b_mat, BLOCK_MATRIX_SIZE, MPI_DOUBLE, rankDestinationShiftMatrixB, 2, rankSourceShiftMatrixB, 2, comm_cart, &statusInitShiftB);
    
    /*
    // Testing:
    // matrixA
    MPI_Barrier(comm_cart);
    printf("matrixA block:\n");
    for(uint64_t i = 0; i < BLOCK_MATRIX_SIZE; i++){
        printf("MATRIX_A: processCoord:(%d, %d) matrix[%d]=%f\n", processCartCoords[0], processCartCoords[1], i, a_mat[i]);
    }
    */
    
    /*
    // matrixB
    MPI_Barrier(comm_cart);
    printf("matrixB block:\n");
    for(uint64_t i = 0; i < BLOCK_MATRIX_SIZE; i++){
        printf("MATRIX_B: processCoord:(%d, %d) matrix[%d]=%f\n", processCartCoords[0], processCartCoords[1], i, b_mat[i]);
    } 
    */



    /* Shift matrices A and B (subtask 2.4) */
    MPI_Barrier(comm_cart);
    MPI_Cart_shift(comm_cart, 1, -1, &rankSourceShiftMatrixA, &rankDestinationShiftMatrixA);
    MPI_Cart_shift(comm_cart, 0, -1, &rankSourceShiftMatrixB, &rankDestinationShiftMatrixB);

    /* Perform local matrix multiply on initial data (subtask 2.5) */
    mult_mat_fast(a_mat, b_mat, c_mat, q, q, q);

    /* Main iteration (subtask 2.6) */
    // Algorithm: (sqrt(p)-1) times the algorithm will do: multiply, add, shift 
    MPI_Barrier(comm_cart);
    for(double i = 0; i < (sqrtP)-1; i++){
        MPI_Sendrecv_replace(a_mat, BLOCK_MATRIX_SIZE, MPI_DOUBLE, rankDestinationShiftMatrixA, 10, rankSourceShiftMatrixA, 10, comm_cart, &statusInitShiftA);
        MPI_Sendrecv_replace(b_mat, BLOCK_MATRIX_SIZE, MPI_DOUBLE, rankDestinationShiftMatrixB, 20, rankSourceShiftMatrixB, 20, comm_cart, &statusInitShiftB);

        double* tmpResultMatrix = malloc( sizeof(double) * BLOCK_MATRIX_SIZE);
        mult_mat_fast(a_mat, b_mat, tmpResultMatrix, q, q, q);
        add_toMat(c_mat, tmpResultMatrix, q, q);
        free(tmpResultMatrix);

        MPI_Cart_shift(comm_cart, 1, -1, &rankSourceShiftMatrixA, &rankDestinationShiftMatrixA);
        MPI_Cart_shift(comm_cart, 0, -1, &rankSourceShiftMatrixB, &rankDestinationShiftMatrixB);
    }
    

    //Testing result-matrix:
    MPI_Barrier(comm_cart);
    printf("matrixC block:\n");
    for(uint64_t i = 0; i < BLOCK_MATRIX_SIZE; i++){
        printf("processCoord:(%d, %d) matrix[%d]=%f\n", processCartCoords[0], processCartCoords[1], i, c_mat[i]);
    }

    /* Verify the results (subtask 2.7) */
    int global_result = 0;
    if( world_rank == 0 ) {
        printf( "Correct: %d\n", global_result );
    }
    
    free(a_mat);
    free(b_mat);
    free(c_mat);
    MPI_Finalize();

    return 0;
}
