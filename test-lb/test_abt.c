#include <stdio.h>
#include "../ompc.h"

double get_clock(void);

double iter_start[256];

void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    iter_start[from] = get_clock();
}

int ompc_main(int argc, char *argv[])
{
    double loop_start = get_clock();

    ompc_loop_divide_conquer(_loop_func_0, 0, NULL, 0, 256, 1, 256);

    for (int i = 0; i < 256; i++) {
        printf("%e\n", iter_start[i] - loop_start);
    }

    return 0;
}
