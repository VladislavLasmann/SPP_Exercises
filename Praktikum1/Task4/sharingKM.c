int count = 0;
double val = 0.0;

double* foo( int c, double* g, double h )
{
  if ( h < c ) return g;
  else return &val;
}
// a) foo(): count, val, g *g
// private: g, *g
// shared: val, count

void bar(double* a, double *z, int n)
{
   static int cnt = 0;
   int i, j;
   
   #pragma omp parallel for firstprivate (cnt, val)
   for( j=0; j<n; j++)
   {
       int* res = foo( count, &a[j], z[j] );
       for (i=0; i<j; i++)
       {
	   cnt++;
       }
       count = count + *res + cnt;
   }
}

// b) bar: count, val, cnt, res, *res, i, j, a
// private: j, cnt, res, val
// shared:  *res, i, count, a
