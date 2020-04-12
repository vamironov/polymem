#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include <mkl.h>
#include <assert.h>
#include <omp.h>

#include "util.h"
#include "poly_driver.h"

int main(int argc, char* argv[])
{
    struct timeval stime, etime, diff, totTime, minTime, maxTime;
    double timeInSec;
    double minTimeInSec;
    double maxTimeInSec;
    double gflop;

    FLOAT_TYPE* restrict P;
    FLOAT_TYPE* restrict A;
    FLOAT_TYPE* restrict B;

    int n = 24;
    long TSIZE = 1024;
    int LOOP_COUNT = 10;

    char *tmp;
    if (argc >= 2) n = atoi(argv[1]);
    if (argc >= 3) {
        TSIZE = strtoll(argv[2],&tmp,0)-1;
        TSIZE = ROUNDTOMUL(TSIZE, TMPLEN*CHUNKSIZE);
    }
    if (argc >= 4) LOOP_COUNT = atoi(argv[3]);

    int npad =  ROUNDTOMUL(n+1, TMPLEN);

    posix_memalign((void *) &P, ALIGN, sizeof(double)*(npad));
    posix_memalign((void *) &A, ALIGN, sizeof(double)*((size_t) TSIZE));
    posix_memalign((void *) &B, ALIGN, sizeof(double)*((size_t) TSIZE));

    printf("\n");
    printf("Polynomial degree             : %d \n", n);
    printf("Test size                     : %ld \n", TSIZE);
    printf("Memory usage for single array : %6.3f GB\n", TSIZE*8.0/1024/1024/1024);
    printf("Memory usage for all arrays   : %6.3f GB\n", 2*TSIZE*8.0/1024/1024/1024);
    printf("Loop count                    : %d \n", LOOP_COUNT);
#if defined(_OPENMP)
    printf("Number of threads             : %d \n", omp_get_max_threads());
#endif

    gflop = 2.0*n*1E-9*TSIZE;
    printf("GFlop                         : %.2f \n", gflop);

    fflush(stdout);

    set_matrix(TSIZE,A);

    for (int i=0; i<npad; i++) P[i]=0.0;
    set_poly(n,P);

    printf("\n --- READ AND WRITE PATTERN ---\n");

    fflush(stdout);

    // Heatup
    gettimeofday(&stime, NULL);

    do_test_rw(n, TSIZE, A, B, P);

    gettimeofday(&etime,NULL);
    timersub(&etime, &stime, &diff);
    timeInSec  = (diff.tv_sec*1000+diff.tv_usec/1000)/1000.0;
    printf("[Heatup time ]: %.1f secs \n", timeInSec);

    fflush(stdout);

    // Production
    timerclear(&totTime);
    timerclear(&maxTime);
    timerclear(&minTime);
    timeradd(&minTime, &diff, &minTime); // Reuse diff
    minTime.tv_sec += 1000000;

    printf("\n%10s%16s%16s\n", "Iter #", "Time, s", "GFlop/s");

    for (int i=0; i<LOOP_COUNT; ++i) {
        gettimeofday(&stime, NULL);

        do_test_rw(n, TSIZE, A, B, P);

        gettimeofday(&etime,NULL);
        timersub(&etime, &stime, &diff);
        timeradd(&totTime, &diff, &totTime);

        if ( timercmp(&maxTime, &diff, <) )
        {
            timerclear(&maxTime);
            timeradd(&maxTime, &diff, &maxTime);
        }
        if ( timercmp(&minTime, &diff, >) )
        {
            timerclear(&minTime);
            timeradd(&minTime, &diff, &minTime);
        }
        timeInSec  = (diff.tv_sec*1000+diff.tv_usec/1000)/1000.0;
        printf("%10d%16.1f%16.2f\n", i+1, timeInSec, gflop/timeInSec);
        fflush(stdout);
    }

    printf("\n");

    minTimeInSec  = (minTime.tv_sec*1000+minTime.tv_usec/1000)/1000.0;
    maxTimeInSec  = (maxTime.tv_sec*1000+maxTime.tv_usec/1000)/1000.0;
    timeInSec     = (totTime.tv_sec*1000+totTime.tv_usec/1000)/1000.0/ LOOP_COUNT;

    printf("[Min time     ]: %.2f secs \n", minTimeInSec);
    printf("[Max time     ]: %.2f secs \n", maxTimeInSec);
    printf("[Avg time     ]: %.2f secs \n", timeInSec);
    printf("[Total time   ]: %.2f secs \n", timeInSec*LOOP_COUNT);
    printf("\n");
    printf("[Min GFlop/sec]: %.3f \n", gflop/maxTimeInSec);
    printf("[Max GFlop/sec]: %.3f \n", gflop/minTimeInSec);
    printf("[Avg GFlop/sec]: %.3f \n", gflop/timeInSec);

    free(P);
    free(A);
    free(B);

    return 0;
}
