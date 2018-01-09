#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "merge_sort.h"

/**
 * Prints an error-message, telling that the argument should be an positive number
 * @return -1
 */
int throwError(){
    printf("Please pass an positive integer number as first and second argument.\n");
    return -1;
}
void printArray(int *array, unsigned int arrayLength) {
    for(int i = 0; i < arrayLength; ++i){
        printf("%d | ", array[i]);
    }
    printf("\n");
}

/**
 * Receives an integer array and its length and fills it with random integer numbers
 * @param array an integer array, to fill the numbers in
 * @param arrayLength length of the array
 */
void fillArrayRandom(int *array, unsigned int arrayLength){
    for(unsigned int i = 0; i < arrayLength; i++){
        array[i] = rand();
        //printf("fillArrayRandom%d\n", array[i]);
    }
}

/**
 * Determines whether the passed array is sorted in ascending order
 * Returns 1, if yes. Otherwise 0
 * @param array the array to check
 * @param arrayLength the array length
 * @return 1 if the array is sorted in ascending order. Otherwise 0
 */
int isSorted(int *array, unsigned int arrayLength){
    for(unsigned int i = 0; i < (arrayLength-1); i++){
        if( array[i] > array[i+1] )
            return 0;
    }
    return 1;
}

/**
 * Merging the halves of the first array by doing binary comparison and taking elements in ascending order
 * and using the given help-array and saving back to the first array
 * Both arrays should have the same size
 * @param array the array, which halves will be merged
 * @param help the help-array
 * @param length the length of the both arrays
 */
void mergeAscendingOrder(int* array, int* help, unsigned int length){
    unsigned int indexArrayHalf1 = 0;
    unsigned int indexArrayHalf2 = length/2;
    unsigned int indexHelpArray = 0;

    // fill in help by binary comparision
    while( (indexArrayHalf1 < length/2) && (indexArrayHalf2 < length) ){
        if( array[indexArrayHalf1] <= array[indexArrayHalf2]){
            help[ indexHelpArray++ ] = array[ indexArrayHalf1++ ];
        }else{
            help[ indexHelpArray++ ] = array[ indexArrayHalf2++];
        }
    }

    //here one array-half is not completly traversed
    while(indexArrayHalf1 < length/2){
        help[ indexHelpArray++ ] = array[ indexArrayHalf1++ ];
    }
    while(indexArrayHalf2 < length){
        help[ indexHelpArray++ ] = array[ indexArrayHalf2++ ];
    }

    // copy every element from help in the array
    for(unsigned int i = 0; i < length; i++){
        array[i] = help[i];
    }

}

/**
 * Parallel merge-sort algorith for sorting elements from the first array in ascending order and using the help-array
 * @param array the array that will be sorted
 * @param help help-array 
 * @param length the length of the both arrays
 * @param taskBreak the upper limit of arrayLength, where the parallel tasks will be taken
 */
void merge_sort( int* array, int* help, unsigned int length, unsigned int taskBreak){
    if(length < 2){
        return;
    }
    // The array lengths
    unsigned int arrayHalf1Length = length / 2;
    unsigned int arrayHalf2Length = length - arrayHalf1Length;
    // pointer for the second half of the arrays
    int* pointerArrayHalf2 = array + arrayHalf1Length;
    int* pointerHelpHalf2 = help + arrayHalf1Length;

    #pragma omp task if ( arrayHalf1Length > taskBreak )
    {
    merge_sort(array, help, arrayHalf1Length, taskBreak);
    }
    #pragma omp task if ( arrayHalf2Length > taskBreak )
    {
    merge_sort(pointerArrayHalf2, pointerHelpHalf2, arrayHalf2Length, taskBreak);
    }
    #pragma omp taskwait
    mergeAscendingOrder(array, help, length);
}


/**
 * Takes an integer as input, that represents a array length.
 * Program have following execution order:
 *  - creating an integer array with the passed length and fills it with random numbers
 *  - sorts the array by using the merge-sort algorithm
 *  - checks whether the processed array is sorted and prints the result
 * @param argc should be 2, because got an integer parameter as input
 * @param argv argv[1] should be an integer, that represents the array length
 * @return 0 if program worked successfully, otherwise -1
 */
int main(int argc, char *argv[]) {

    double programStartTime = omp_get_wtime();

    // not three arguments -> error
    if(argc != 3) {
        return throwError();
    }

    int enteredArrayLength = strtol(argv[1], &argv[1], 0);
    int enteredTaskBreak = strtol(argv[2], &argv[2], 0);
    //printf("EnteredValue: %d\n", enteredArrayLength);

    // checking arguments
    if( enteredArrayLength <= 0 || enteredTaskBreak <= 0 ) {
        return throwError();
    }

    int *theArray = (int*) malloc(sizeof(int) * enteredArrayLength );
    int *helpArray = (int*) malloc(sizeof(int) * enteredArrayLength );

    fillArrayRandom(theArray, enteredArrayLength);
    //printArray(theArray, enteredArrayLength);

    double algorithmStartTime = omp_get_wtime();
    // number of threads in the parellisation: 4
    // merge_sort will called only on time, by single-clause
    #pragma omp parallel num_threads(4)
    {
        #pragma omp single
        {
            // passing the taskbreak
            merge_sort(theArray, helpArray, enteredArrayLength, enteredTaskBreak);
        }
    }
    printf("sorting-time: %g seconds \n", omp_get_wtime() - algorithmStartTime );
    printf("is array sorted? %s\n", isSorted(helpArray, enteredArrayLength)? "True" : "False");
    
    //printArray(helpArray, enteredArrayLength);
    free(theArray);
    free(helpArray);
    return 0;
}

