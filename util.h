#ifndef _POLY_UTIL
#define _POLY_UTIL

#define FLOAT_TYPE double

#ifdef __INTEL_COMPILER
#define TMPLEN 8
#else
#define TMPLEN 64
#endif

#define ALIGN  64
#define CHUNKSIZE 100

#define ROUNDTOMUL(a,b) ( ( ( (a) - 1) / (b) + 1 ) * (b) )

#define MAX(a,b) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define L2_LEN ( 1024*1024/sizeof(FLOAT_TYPE) )
#define LB_LEN ( MAX( L2_LEN/(3*CHUNKSIZE*TMPLEN), 1 ) )

typedef FLOAT_TYPE VARR[TMPLEN];

FLOAT_TYPE myrand();

void set_poly(const int n, FLOAT_TYPE * const restrict P);

void print_poly(const int n, const FLOAT_TYPE *P);

void set_matrix(const size_t n, FLOAT_TYPE * const restrict A);

#endif/*_POLY_UTIL*/

