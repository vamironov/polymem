#include <stdlib.h>
#include "util.h"
#include "poly_driver.h"

#ifdef __INTEL_COMPILER

//#pragma omp declare simd linear(x:1) uniform(n,P) simdlen(TMPLEN)
FLOAT_TYPE poly(const int n, const FLOAT_TYPE x, const FLOAT_TYPE * restrict P)
{
    FLOAT_TYPE sum = 0.0;
    for (int i=0; i<=n; i++) {
        sum = sum*x + P[i];
    }
    return sum;
}


void do_test_rw(const int n, const size_t TSIZE,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict B,
        const FLOAT_TYPE * const restrict P)
{
    __assume_aligned(A,ALIGN);
    __assume_aligned(B,ALIGN);
    __assume_aligned(P,ALIGN);
#pragma omp parallel for schedule(static,LB_LEN)
    for (size_t j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
        for (size_t j0=j; j0<j+TMPLEN*CHUNKSIZE; j0+=TMPLEN) {
            #pragma omp simd
            for (size_t k=0; k<TMPLEN; k++) {
                B[j0+k] = poly(n,A[j0+k],P);
            }
        }
    }
}

void do_test_ro(const int n, const size_t TSIZE,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * restrict sum,
        const FLOAT_TYPE * const restrict P)
{
    __assume_aligned(A,ALIGN);
    __assume_aligned(P,ALIGN);
    FLOAT_TYPE tsum = 0.0;
#pragma omp parallel reduction(+:tsum)
    {
        VARR buf = {0.0};
#pragma omp for schedule(static,LB_LEN)
        for (size_t j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
            for (size_t j0=j; j0<j+TMPLEN*CHUNKSIZE; j0+=TMPLEN) {
                #pragma omp simd
                for (size_t k=0; k<TMPLEN; k++) {
                    buf[k]+= poly(n,A[j0+k],P);
//                    tsum+= poly(n,A[j0+k],P);
                }
            }
        }
        for (int v=0; v<TMPLEN; v++) tsum += buf[v];
    }

    *sum += tsum;
}

#elif defined __GNUC__

void poly(const int n, VARR sum, const VARR x, const FLOAT_TYPE * restrict P)
{
    __builtin_assume_aligned(x,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
    __builtin_assume_aligned(sum,ALIGN);
    VARR tmp __attribute__ ((aligned (ALIGN))) = {0.0};
    for (int i=0; i<=n; i++) {
        for (int v=0; v<TMPLEN; v++) {
            tmp[v] = tmp[v]*x[v] + P[i];
        }
    }
    for (int v=0; v<TMPLEN; v++) sum[v] += tmp[v];
}

void do_test_rw(const int n, const size_t TSIZE,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict B,
        const FLOAT_TYPE * const restrict P)
{
    __builtin_assume_aligned(A,ALIGN);
    __builtin_assume_aligned(B,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
#pragma omp parallel for schedule(static,LB_LEN)
    for (size_t j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
        for (size_t k=j; k<j+TMPLEN*CHUNKSIZE; k+=TMPLEN) {
            poly(n,&B[k],&A[k],P);
        }
    }
}

void do_test_ro(const int n, const size_t TSIZE,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict sum,
        const FLOAT_TYPE * const restrict P)
{
    __builtin_assume_aligned(A,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
    FLOAT_TYPE tmp[TMPLEN] __attribute__ ((aligned (ALIGN))) = {0.0};
#pragma omp parallel for schedule(static,LB_LEN) reduction(+:tmp[:TMPLEN])
    for (size_t j=0; j<TSIZE; j+=TMPLEN*CHUNKSIZE) {
        for (size_t k=j; k<j+TMPLEN*CHUNKSIZE; k+=TMPLEN) {
            poly(n,tmp,&A[k],P);
        }
    }
    for (int v=0; v<TMPLEN; v++) *sum += tmp[v];
}

#endif
