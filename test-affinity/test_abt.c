#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <abt.h>
#include "../ompc.h"

int bindings[256];

void _loop_func_0(uint64_t from, uint64_t to_exclusive, int step, void *args[])
{
    cpu_set_t cpuset;
    pthread_t self = pthread_self();
    pthread_getaffinity_np(self, sizeof(cpu_set_t), &cpuset);

    int rank;
    ABT_xstream_self_rank(&rank);
    for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &cpuset)) {
            bindings[rank] = i;
            break;
        }
    }
}

int ompc_main(int argc, char *argv[])
{
    int num_threads = ompc_get_max_threads();

    ompc_loop_divide_conquer(_loop_func_0, 0, NULL, 0, 256, 1, 256);

    for (int i = 0; i < num_threads; i++) {
        printf("%d\n", bindings[i]);
    }

    return 0;
}
