#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include <mkl.h>
#include <omp.h>

#include "util.h"
#include "poly_driver.h"

int main(int argc, char* argv[])
{
    struct timeval stime, etime, diff;
    double timeinsec;
    //double time_st, time_end, time_avg;
    double gflop;

    FLOAT_TYPE* restrict P;
    FLOAT_TYPE* restrict A;

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

    posix_memalign((void *) &P, ALIGN, sizeof(double)*(n+1));
    posix_memalign((void *) &A, ALIGN, sizeof(double)*((size_t) TSIZE));

    printf("\n");
    printf("Polynomial degree             : %d \n", n);
    printf("Test size                     : %ld \n", TSIZE);
    printf("Memory usage for single array : %6.3f GB\n", TSIZE*8.0/1024/1024/1024);
    printf("Memory usage for all arrays   : %6.3f GB\n", TSIZE*8.0/1024/1024/1024);
    printf("Loop count                    : %d \n", LOOP_COUNT);
    printf("Number of threads             : %d \n", omp_get_max_threads());
    fflush(stdout);

    set_matrix(TSIZE,A);

    set_poly(n,P);

    printf("\n --- READ ONLY PATTERN ---\n");

    fflush(stdout);

    FLOAT_TYPE sum[TMPLEN] = {0.0};
    // Heatup
    gettimeofday(&stime, NULL);

    do_test_ro(n, TSIZE, 1, A, sum, P);

    gettimeofday(&etime,NULL);
    timersub(&etime, &stime, &diff);
    timeinsec  = (diff.tv_sec*1000+diff.tv_usec/1000)/1000.0;
    printf("[Heatup time ]: %.1f secs \n", timeinsec);

    fflush(stdout);

    // Production
    gettimeofday(&stime, NULL);

    do_test_ro(n, TSIZE, LOOP_COUNT, A, sum, P);

    gettimeofday(&etime,NULL);
    timersub(&etime, &stime, &diff);
    timeinsec  = (diff.tv_sec*1000+diff.tv_usec/1000)/1000.0/ LOOP_COUNT;

    gflop = 2.0*n*1E-9*TSIZE;

    printf("[Average time]: %.1f secs \n", timeinsec);
    printf("[Total time  ]: %.1f secs \n", timeinsec*LOOP_COUNT);
    printf("[GFlop       ]: %.2f \n", gflop);
    printf("[GFlop/sec   ]: %.2f \n", gflop/timeinsec);

    free(P);
    free(A);

    return 0;
}
