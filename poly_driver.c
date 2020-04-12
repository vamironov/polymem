#include <stdlib.h>
#include "util.h"
#include "poly_driver.h"

void poly(const int n, VARR sum, const VARR x, const FLOAT_TYPE * restrict P)
{
    __builtin_assume_aligned(x,ALIGN);
    __builtin_assume_aligned(P,ALIGN);
    __builtin_assume_aligned(sum,ALIGN);
    VARR tmp __attribute__ ((aligned (ALIGN))) = {0.0};
    for (int i=0; i<=n; i++) {
#ifdef __INTEL_COMPILER
#pragma omp simd
#endif
        for (int v=0; v<TMPLEN; v++) {
            tmp[v] = tmp[v]*x[v] + P[i];
        }
    }
#ifdef __INTEL_COMPILER
#pragma omp simd
#endif
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
