#include <stddef.h>
#include <string.h>

void daxpy(size_t n, double a, double x[restrict], double y[restrict]);

void dgemm(size_t l, size_t m, size_t n, double A[restrict][m], double B[restrict][n], double C[restrict][n])
{
#ifdef SCHED_CYCLIC
    #pragma omp parallel for schedule(static, 1) firstprivate(m, n, A, B, C)
#else
    #pragma omp parallel for schedule(static) firstprivate(m, n, A, B, C)
#endif
    for (size_t i = 0; i < l; i++) {
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
