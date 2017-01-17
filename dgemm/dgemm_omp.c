#include <stddef.h>
#include <string.h>
#include <omp.h>

void daxpy(size_t n, double a, double x[restrict], double y[restrict]);
double get_clock(void);

#ifdef ENABLE_LOGGING
double iter_start[256];
double iter_end[256];
#endif

void dgemm(size_t l, size_t m, size_t n, double A[restrict][m], double B[restrict][n], double C[restrict][n])
{
    #pragma omp parallel firstprivate(m, n, A, B, C)
    {
#ifdef ENABLE_LOGGING
        int id = omp_get_thread_num();
        iter_start[id] = get_clock();
#endif

#ifdef SCHED_CYCLIC
        #pragma omp for schedule(static, 1)
#else
        #pragma omp for schedule(static)
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

#ifdef ENABLE_LOGGING
        iter_end[id] = get_clock();
#endif
    }
}
