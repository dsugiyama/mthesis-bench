#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*loop_func_t)(uint64_t from, uint64_t to_exclusive, int step, void *args[]);
void ompc_loop_divide_conquer(loop_func_t func, int nargs, void *args,
                              uint64_t from, uint64_t to_exclusive, int step, int num_tasks);
double get_clock(void);
int delay_length_from_sec(double delay_sec);
void delay(int delay_length);

void _loop_func_1(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    int delay_length = *(int *) args[0];

    for (int j = from; j < to_exclusive; j++) {
        delay(delay_length);  // simulated work
    }
}

void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    int NTHRDS = *(int *) args[0];
    int delay_length = *(int *) args[1];

    for (int i = from; i < to_exclusive; i++) {
        void *_loop_params_1[] = { &delay_length };
        ompc_loop_divide_conquer(_loop_func_1, 1, _loop_params_1, 0, NTHRDS, 1, NTHRDS);
    }
}

int main(int argc, char *argv[])
{
    int NTHRDS = atoi(argv[1]);
    int DELAY_USEC = atoi(argv[2]);

    int delay_length = delay_length_from_sec(DELAY_USEC * 1E-6);

    void *_loop_params_0[] = { &NTHRDS, &delay_length };
    ompc_loop_divide_conquer(_loop_func_0, 2, _loop_params_0, 0, NTHRDS, 1, NTHRDS);

    double start = get_clock();

    ompc_loop_divide_conquer(_loop_func_0, 2, _loop_params_0, 0, NTHRDS, 1, NTHRDS);

    double elapsed = get_clock() - start;
    printf("%f sec\n", elapsed);
}
