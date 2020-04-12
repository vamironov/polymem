#include "util.h"

void poly(const int n, VARR sum, const VARR x, const FLOAT_TYPE * restrict P);

void do_test_rw(const int n, const size_t TSIZE,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict B,
        const FLOAT_TYPE * const restrict P);

void do_test_ro(const int n, const size_t TSIZE,
        const FLOAT_TYPE * const restrict A,
              FLOAT_TYPE * const restrict sum,
        const FLOAT_TYPE * const restrict P);
