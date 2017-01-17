#include <stddef.h>
#include "../ompc.h"

void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    double a = *(double *) args[0];
    double *restrict x = args[1];
    double *restrict y = args[2];

    for (size_t i = from; i < to_exclusive; i++) {
        y[i] = a * x[i] + y[i];
    }
}

void daxpy(size_t n, double a, double x[restrict], double y[restrict])
{
    void *_loop_args_0[] = { &a, x, y };
    ompc_loop_divide_conquer(_loop_func_0, 3, _loop_args_0, 0, n, 1, 32);
}
