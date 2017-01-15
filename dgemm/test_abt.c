#include <stdio.h>
#include <stdlib.h>
#include "ompc.h"

void dgemm(size_t l, size_t m, size_t n, double A[restrict][m], double B[restrict][n], double C[restrict][n]);
double get_clock(void);

_Thread_local unsigned int seed = 0;

static void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    size_t N = *(size_t *) args[0];
    double (*restrict A)[N] = args[1];

    for (size_t i = from; i < to_exclusive; i++) {
        for (size_t j = 0; j < N; j++) {
            A[i][j] = rand_r(&seed);
        }
    }
}

int ompc_main(int argc, char *argv[])
{
    size_t L = atoi(argv[1]);
    size_t M = atoi(argv[2]);
    size_t N = atoi(argv[3]);

    double (*restrict A)[M] = malloc(sizeof(double [L][M]));
    double (*restrict B)[N] = malloc(sizeof(double [M][N]));
    double (*restrict C)[N] = malloc(sizeof(double [L][N]));

    void *_loop_args_0[] = { &M, A };
#ifdef SCHED_CYCLIC
    ompc_loop_divide_conquer(_loop_func_0, 2, _loop_args_0, 0, L, 1, L);
#else
    ompc_loop_divide_conquer(_loop_func_0, 2, _loop_args_0, 0, L, 1, ompc_get_max_threads());
#endif

    void *_loop_args_1[] = { &N, B };
#ifdef SCHED_CYCLIC
    ompc_loop_divide_conquer(_loop_func_0, 2, _loop_args_1, 0, M, 1, M);
#else
    ompc_loop_divide_conquer(_loop_func_0, 2, _loop_args_1, 0, M, 1, ompc_get_max_threads());
#endif

    double t1 = get_clock();
    dgemm(L, M, N, A, B, C);
    double t2 = get_clock();
    printf("%f sec\n", t2 - t1);

    size_t x = rand_r(&seed) % L, y = rand_r(&seed) % N;
    printf("C[%zu][%zu] = %f\n", x, y, C[x][y]);

    free(A);
    free(B);
    free(C);

    return 0;
}
