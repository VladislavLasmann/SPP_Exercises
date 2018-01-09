//
// Created by Konstantin Müller on 28.11.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// takes value from argv[1]
unsigned length;
// initiate in main
int* array;
// initiate in main
int* help;

/**
 * Merge sort splits int* array in half until one Element remains
 * then fills int* help in ascendant sequence
 * @param array
 * @param help to fill in ascendant sequence
 * @param length should be same size like int* array and help
 * @return help
 */
int* merge_sort(int* array, int* help, unsigned length) {

	if(length < 2)
		return array = help;

	int half1[length/2];
	int half2[(length + 1)/2];
	int i;

	for(i = 0; i < length/2; ++i)
		half1[i] = array[i];

	for(i = length/2; i < length; ++i)
		half2[i - length/2] = array[i];

	merge_sort(half1, help, length/2);
	merge_sort(half2, help, (length + 1)/2);

	int* pos1 = &half1[0];
	int* pos2 = &half2[0];
	for(i = 0; i < length; ++i) {
		if(*pos1 <= *pos2) {
			// add to help
			help[i] = *pos1;
			// pos1 don't changes if the biggest value appears several times
			if (pos1 != &half2[(length+1)/2 - 1])
				if(pos1 == &half1[length/2 - 1])
					pos1 = &half2[(length+1)/2 - 1];
				else ++pos1;
		}
		else { help[i] = *pos2;

			if(pos2 == &half2[(length + 1)/2 - 1])
				pos2 = &half1[length/2 - 1];
			else ++pos2;
		}
	}
}

/**
 * Allocates array und initiate it with random positive numbers
 * @param array expected an declarated int* array
 * @param length of array
 * @return int* array
 */
int* init_rand_array( unsigned length) {

	int* array = malloc(sizeof(int) * length);
	srand(time(NULL));

	for(int i = 0; i < length; i++) {
		array[i] = rand();
	}
	return array;
}

/**
 * Allocates empty array
 * @param length of array
 * @return int* array
 */
int* init_emp_array(unsigned length) {
	int* array = malloc(sizeof(int) * length);
	return array;
}

/**
 * Print the array
 * @param array
 */
void print_array(int* array) {
	printf("A[");
	for(int i = 0; i < length-1; i++) {
		if (i % 10 == 0 && i != 0)
			printf("%d;\n", array[i]);
		else printf("%d;  ", array[i]);
	}
	printf("%d]\n\n", array[length-1]);
}

/**
 * Checks int* array whether ascendant
 * @param array
 * @return 1 if ascendant else 0
 */
int is_ascendant(int* array) {
	for(int i = 1; i < length; i++) {
		if(array[i-1] > array[i])
			return 0;
	}
	return 1;
}

int main(int argc, char** argv){

	length = strtol(argv[1],NULL, 10);

	printf("%d\n", length);
	printf("%d\n", sizeof(int));

	array = init_rand_array(length);
	// array before merge
	print_array(array);

	help = init_emp_array(length);

	merge_sort(array, help, length);
	// array after merge
	print_array(array);

	printf("Array is ascendant (true = 1; false = 0): %d\n", is_ascendant(array));

	free(array);
	free(help);
}