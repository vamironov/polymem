#include <stdlib.h>
#include "util.h"
#include "poly_driver.h"

#ifdef __INTEL_COMPILER

//#pragma omp declare simd linear(x:1) uniform(n,P) simdlen(TMPLEN)
FLOAT_TYPE poly(const int n, const FLOAT_TYPE x, const FLOAT_TYPE * restrict P)
{
    FLOAT_TYPE sum;
    sum = P[0];
    int i;
    for (i=1; i<=n; i++) {
        sum = sum*x + P[i];
    }
    return sum;
}


void do_test_rw(const int n, const size_t TSIZE, const int LOOP_COUNT,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict B,
        const FLOAT_TYPE * const restrict P)
{
    __builtin_assume_aligned(A,ALIGN);
    __builtin_assume_aligned(B,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
#pragma omp parallel
    {
        int i;
        size_t j, j0, k;
        FLOAT_TYPE buf[TMPLEN*CHUNKSIZE];
        for (i=0; i<LOOP_COUNT; ++i) {
#pragma omp for schedule(static,LB_LEN)
            for (j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
                for (j0=j; j0<j+TMPLEN*CHUNKSIZE; j0+=TMPLEN) {
                    #pragma omp simd
                    for (k=0; k<TMPLEN; k++) {
                        buf[j0-j+k] = poly(n,A[j0+k],P);
                        //B[j0+k] = poly(n,A[j0+k],P);
                    }
                }
                for (k=0; k<TMPLEN*CHUNKSIZE; k++) {
                    B[j+k] = buf[k];
                }
            }
        }
    }
}

void do_test_ro(const int n, const size_t TSIZE, const int LOOP_COUNT,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict sum,
        const FLOAT_TYPE * const restrict P)
{
    __builtin_assume_aligned(A,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
#pragma omp parallel
    {
        int i;
        size_t j, j0, k;
        VARR buf = {0.0};
        for (i=0; i<LOOP_COUNT; ++i) {
#pragma omp for schedule(static,LB_LEN)
            for (j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
                for (j0=j; j0<j+TMPLEN*CHUNKSIZE; j0+=TMPLEN) {
                    #pragma omp simd
                    for (k=0; k<TMPLEN; k++) {
                        buf[k]+= poly(n,A[j0+k],P);
                    }
                }
            }
        }
        for (k = 0; k<TMPLEN; k++) {
#pragma omp atomic
            sum[k]+= buf[k];
        }
    }
}

#elif defined __GNUC__

void poly(const int n, VARR sum, const VARR x, const FLOAT_TYPE * restrict P)
{
    for (int v=0; v<TMPLEN; v++) sum[v] = 0.0;

    for (int i=0; i<=n; i++) {
        for (int v=0; v<TMPLEN; v++) {
            sum[v] = sum[v]*x[v] + P[i];
        }
    }
}

void do_test_rw(const int n, const size_t TSIZE, const int LOOP_COUNT,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict B,
        const FLOAT_TYPE * const restrict P)
{
    __builtin_assume_aligned(A,ALIGN);
    __builtin_assume_aligned(B,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
#pragma omp parallel
    {
        int i;
        size_t j, j0, k;
        for (i=0; i<LOOP_COUNT; ++i) {
#pragma omp for schedule(static,LB_LEN)
            for (j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
                for (j0=j; j0<j+TMPLEN*CHUNKSIZE; j0+=TMPLEN) {
                    poly(n,&B[k],&A[k],P);
                }
            }
        }
    }
}

void do_test_ro(const int n, const size_t TSIZE, const int LOOP_COUNT,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict sum,
        const FLOAT_TYPE * const restrict P)
{
    __builtin_assume_aligned(A,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
#pragma omp parallel
    {
        int i;
        size_t j, j0, k;
        VARR buf = {0.0};
        for (i=0; i<LOOP_COUNT; ++i) {
#pragma omp for schedule(static,LB_LEN)
            for (j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
                for (j0=j; j0<j+TMPLEN*CHUNKSIZE; j0+=TMPLEN) {
                    poly(n,buf,&A[j0],P);
                }
            }
        }
        for (k = 0; k<TMPLEN; k++) {
#pragma omp atomic
            sum[k]+= buf[k];
        }
    }
}

#endif
