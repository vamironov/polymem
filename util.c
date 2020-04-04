#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "util.h"

FLOAT_TYPE myrand()
{
    return rand()/(1.0*RAND_MAX)-0.5;
}

void set_poly(const int n, FLOAT_TYPE * const restrict P)
{
    int i;
    srand(time(NULL));
    P[0] = 0.0;
    for (i=1; i<=n ; ++i)
    {
        P[i] = myrand()/n;
    }
}

void print_poly(const int n, const FLOAT_TYPE *P)
{
    int i;
    printf("Sum = ");
    for (i=0; i<=n ; ++i)
    {
        printf("%+10.4e*x^%i ",P[i],i);
    }
    printf("\n");
}

void set_matrix(const size_t n, FLOAT_TYPE * const restrict A)
{
    size_t j, k;
    const FLOAT_TYPE value = 1.0;
    /* Parallel initialization to use "first-touch"
     * memory binding to threads */
    printf("%li %li\n", n, (size_t)TMPLEN*CHUNKSIZE);
    assert(!( n % (TMPLEN*CHUNKSIZE)) );
#pragma omp parallel for private(j,k) schedule(static,LB_LEN)
    for (j = 0;  j < n;  j += TMPLEN*CHUNKSIZE) {
        for (k = j;  k < j+TMPLEN*CHUNKSIZE;  k++) {
            A[k] = value;
        }
    }
}
