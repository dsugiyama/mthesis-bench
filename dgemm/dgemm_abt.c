#include <stddef.h>
#include <string.h>
#include "../ompc.h"

void daxpy(size_t n, double a, double x[restrict], double y[restrict]);

static void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
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
}

void dgemm(size_t l, size_t m, size_t n, double A[restrict][m], double B[restrict][n], double C[restrict][n])
{
    void *_loop_args_0[] = { &m, &n, A, B, C };
#ifdef SCHED_CYCLIC
    ompc_loop_divide_conquer(_loop_func_0, 5, _loop_args_0, 0, l, 1, l);
#else
    ompc_loop_divide_conquer(_loop_func_0, 5, _loop_args_0, 0, l, 1, ompc_get_max_threads());
#endif
}
