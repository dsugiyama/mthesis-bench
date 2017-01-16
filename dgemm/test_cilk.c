#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

void dgemm(size_t l, size_t m, size_t n, double A[restrict][m], double B[restrict][n], double C[restrict][n]);
double get_clock(void);

_Thread_local unsigned int seed = 0;

int main(int argc, char *argv[])
{
    size_t L = atoi(argv[1]);
    size_t M = atoi(argv[2]);
    size_t N = atoi(argv[3]);

    double (*restrict A)[M] = malloc(sizeof(double [L][M]));
    double (*restrict B)[N] = malloc(sizeof(double [M][N]));
    double (*restrict C)[N] = malloc(sizeof(double [L][N]));

#ifdef SCHED_CYCLIC
    #pragma cilk grainsize = 1
#else
    #pragma cilk grainsize = L / __cilkrts_get_nworkers()
#endif
    cilk_for (size_t i = 0; i < L; i++) {
        for (size_t j = 0; j < M; j++) {
            A[i][j] = rand_r(&seed);
        }
    }

#ifdef SCHED_CYCLIC
    #pragma cilk grainsize = 1
#else
    #pragma cilk grainsize = M / __cilkrts_get_nworkers()
#endif
    cilk_for (size_t i = 0; i < M; i++) {
        for (size_t j = 0; j < N; j++) {
            B[i][j] = rand_r(&seed);
        }
    }

    double t1 = get_clock();
    dgemm(L, M, N, A, B, C);
    double t2 = get_clock();
    printf("%f sec\n", t2 - t1);

    size_t x = rand_r(&seed) % L, y = rand_r(&seed) % N;
    printf("C[%zu][%zu] = %f\n", x, y, C[x][y]);

    free(A);
    free(B);
    free(C);
}
