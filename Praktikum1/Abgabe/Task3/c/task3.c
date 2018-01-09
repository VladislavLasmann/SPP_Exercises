/**
 * Gruppe: 122
 * Konstantin Müller (2327697) 
 * Robin Ferrari 	 (2585277) 
 * Vladislav Lasmann (2593078)
 */
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define LENGTH 1000000

double stime;
double etime;

/**
 * Sums up all elements of an array
 * @param a int array
 * @param length of array
 * @return sum of all elements
 */
int sum( int* a, unsigned length )
{
    int result = 0;
	// a) result hat an output dependence
    for( unsigned i=0; i<length; i++ )
    {
        result += a[i];
    }
    return result;
}

/**
 * Shifts last half of elements from array to the first half
 * @param a
 * @param length
 * @param offset
 */
void shift( int* a, unsigned length, unsigned offset )
{   // a) var a[] has no dependence
	// because of write(a[i]) in the first half array
	// and read(a[i+length]) in the last half
    for( unsigned i=0; i<length-offset; i++ )
    {
        a[i] = a[i+offset];
    }
	// a) var ai has no dependence
	// because a[i] is not loop carried
    for( unsigned i=length-offset; i<length; i++)
    {
        a[i] = 0;
    }
}

/**
 *
 * @param a
 * @param length
 * @return
 */
int hash( int* a, unsigned length )
{
    int hash = 1;
	// a) var hash has an output dependence
    for( unsigned i=0; i<length; i++ )
    {
        switch( hash % 3 )
	        {
	        case 0:
	            hash += a[i];
	            break;
	        case 1:
	            hash *= a[i];
	            break;
	        default:
	            hash -= a[i];
	}
	hash %= 1000;
    }
    return hash; 
}

void init( int* a, unsigned length, int base, int offset )
{
	// a) no dependence
    for( unsigned i=0; i<length; i++ )
    {
        a[i] = base + i * offset;
    }
}

int main()
{
	stime = omp_get_wtime();
	int result = 0;
	
#pragma omp parallel
	{
	#pragma omp single
		{
			#pragma omp task shared(result)
            {
				//printf("TaskA, threadnumber: %d\n", omp_get_thread_num() );
				int *a = (int*) malloc(LENGTH * sizeof(int));
				init(a, LENGTH, 1, 1);
                result += sum( a, LENGTH );
                shift( a, LENGTH, 1000 );
                result += hash( a, LENGTH );
				free(a);
            }

            #pragma omp task shared(result)
            {
				//printf("TaskB, threadnumber: %d\n", omp_get_thread_num() );
				int *b = (int*) malloc(LENGTH * sizeof(int));
				init(b, LENGTH, 2, 2);
                shift(b, LENGTH, 2);
                result += sum(b, LENGTH);
                result += hash(b, LENGTH);
				free(b);
            }

            #pragma omp task shared(result)
            {
				//printf("TaskC, threadnumber: %d\n", omp_get_thread_num() );
				int *c = (int*) malloc(LENGTH * sizeof(int));
				init(c, LENGTH, 1, 1);
                result += hash( c, LENGTH );
				free(c);
            }

            #pragma omp task shared(result)
            {
				//printf("TaskD, threadnumber: %d\n", omp_get_thread_num() );
				int *d = (int*) malloc(LENGTH * sizeof(int));
				init(d, LENGTH, 0, -1);
                shift( d, LENGTH, 623 );
                result += hash( d, LENGTH );
				free(d);
            }
		}
    }
		etime = omp_get_wtime();
        printf("Time: %.5f\n", etime - stime);
		printf("Result: %d\n", result);
        return 0;
}
