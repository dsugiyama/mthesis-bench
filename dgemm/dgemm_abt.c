#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <abt.h>
#include "../ompc.h"

void daxpy(size_t n, double a, double x[restrict], double y[restrict]);
double get_clock(void);

#ifdef ENABLE_LOGGING
double iter_start[256];
double iter_end[256];
#endif

static void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
#ifdef ENABLE_LOGGING
    int rank;
    ABT_xstream_self_rank(&rank);
    iter_start[rank] = get_clock();
#endif

    size_t m = *(size_t *) args[0];
    size_t n = *(size_t *) args[1];
    double (*restrict A)[m] = args[2];
    double (*restrict B)[n] = args[3];
    double (*restrict C)[n] = args[4];

    for (size_t i = from; i < to_exclusive; i++) {
        memset(C[i], 0, sizeof(double [n]));
        for (size_t k = 0; k < m; k++) {
#ifdef LIB_DAXPY
            daxpy(n, A[i][k], B[k], C[i]);
#else
            for (size_t j = 0; j < n; j++) {
                C[i][j] += A[i][k] * B[k][j];
            }
#endif
        }
    }

#ifdef ENABLE_LOGGING
    iter_end[rank] = get_clock();
#endif
}

void dgemm(size_t l, size_t m, size_t n, double A[restrict][m], double B[restrict][n], double C[restrict][n])
{
    void *_loop_args_0[] = { &m, &n, A, B, C };
#ifdef LIB_DAXPY
    extern int nults_inner;
    nults_inner = atoi(getenv("NULTS_INNER"));
    ompc_loop_divide_conquer(_loop_func_0, 5, _loop_args_0, 0, l, 1, atoi(getenv("NULTS_OUTER")));
#elif defined SCHED_CYCLIC
    ompc_loop_divide_conquer(_loop_func_0, 5, _loop_args_0, 0, l, 1, l);
#else
    ompc_loop_divide_conquer(_loop_func_0, 5, _loop_args_0, 0, l, 1, ompc_get_max_threads());
#endif
}
