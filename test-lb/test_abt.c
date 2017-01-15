#include <stdio.h>
#include "../ompc.h"

double get_clock(void);
int delay_length_from_sec(double delay_sec);
void delay(int delay_length);

int delay_length;
double iter_start[256];

void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    iter_start[from] = get_clock();
    for (int i = 0; i < 100; i++) {
        delay(delay_length);
    }
}

int ompc_main(int argc, char *argv[])
{
    delay_length = delay_length_from_sec(0.001);

    double loop_start = get_clock();
    ompc_loop_divide_conquer(_loop_func_0, 0, NULL, 0, 256, 1, 256);

    for (int i = 0; i < 256; i++) {
        printf("%e\n", iter_start[i] - loop_start);
    }

    return 0;
}
