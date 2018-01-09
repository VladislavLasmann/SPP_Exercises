#ifndef MERGE_SORT_H
#define MERGE_SORT_H

int throwError();
void fillArrayRandom(int *array, unsigned int arrayLength);
int isSorted(int *array, unsigned int arrayLength);
void mergeAscendingOrder(int* array, int* help, unsigned int length);
void merge_sort( int *array, int* help, unsigned length );

#endif //MERGE_SORT_H
