#include <stddef.h>

void daxpy(size_t n, double a, double x[restrict], double y[restrict])
{
    #pragma omp parallel for schedule(static) firstprivate(a, x, y)
    for (size_t i = 0; i < n; i++) {
        y[i] = a * x[i] + y[i];
    }
}
