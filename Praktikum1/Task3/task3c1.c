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
	int *a;
	int *b;
	int *c;
	int *d;
	int result;
#pragma omp parallel
	{
	#pragma omp single
		{
		#pragma omp task depend(out: a) shared(a)
			a = malloc(LENGTH * sizeof(int));
		#pragma omp task depend(inout: a) shared(a)
			init(a, LENGTH, 1, 1);
		#pragma omp task depend(in: a) depend(out: result) shared(a, result)
			result = sum(a, LENGTH);
		#pragma omp task depend(inout: a) shared(a)
			shift(a, LENGTH, 1000);
		#pragma omp task depend(in: a) depend(out: result) shared(a, result)
			result += hash(a, LENGTH);
		#pragma omp task depend(out: a) shared(a)
			free(a);


		#pragma omp task depend(out: b) shared(b)
			b = malloc(LENGTH * sizeof(int));
		#pragma omp task depend(inout: b) shared(b)
			init(b, LENGTH, 2, 2);
		#pragma omp task depend(inout: b) shared(b)
			shift(b, LENGTH, 2);
		#pragma omp task depend(in: b) depend(out: result) shared(b, result)
			result += sum(b, LENGTH);
		#pragma omp task depend(in: b) depend(out: result) shared(b, result)
			result += hash(b, LENGTH);
		#pragma omp task depend(out: b) shared(b)
			free(b);


		#pragma omp task depend(out: c) shared(c)
			c = malloc(LENGTH * sizeof(int));
		#pragma omp task depend(inout: c) shared(c)
			init(c, LENGTH, 1, 1);
		#pragma omp task depend(in:c) depend(out: result) shared(c, result)
			result += hash(c, LENGTH);
		#pragma omp task depend(out: c) shared(c)
			free(c);

			
		#pragma omp task depend(out: d) shared(d)
			d = malloc(LENGTH * sizeof(int));
		#pragma omp task depend(inout: d) shared(d)
			init(d, LENGTH, 0, -1);
		#pragma omp task depend(inout: d) shared(d)
                shift(d, LENGTH, 623);
        #pragma omp task depend(in: d) depend(out: result) shared(d, result)
		        result += hash(d, LENGTH);
		#pragma omp task depend(out: d) shared(d)
				free(d);
		}
    }
		etime = omp_get_wtime();
        printf("Time: %.5f\n", etime - stime);
		printf("Result: %d\n", result);
        return 0;
}
