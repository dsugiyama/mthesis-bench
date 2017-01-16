#include <stdlib.h>
#include <abt.h>
#include "ompc.h"

int ompc_main(int argc, char *argv[]);

static int num_threads = 1;
static ABT_xstream xstreams[256];

int main(int argc, char *argv[])
{
    char *num_threads_env = getenv("OMPC_NUM_PROCS");
    if (num_threads_env != NULL) {
        num_threads = atoi(num_threads_env);
    }

    ABT_init(argc, argv);

    ABT_xstream_self(&xstreams[0]);
    for (int i = 1; i < num_threads; i++) {
        ABT_xstream_create_basic(ABT_SCHED_DEFAULT, 1, NULL, ABT_SCHED_CONFIG_NULL, &xstreams[i]);
    }

    return ompc_main(argc, argv);
}

int ompc_get_max_threads(void)
{
    return num_threads;
}

typedef void (*loop_func_t)(uint64_t from, uint64_t to_exclusive, int step, void *args[]);

static void loop_schedule(void *args[])
{
    loop_func_t func = args[0];
    void *func_args = args[1];
    // assume from == 0
    uint64_t to_exclusive = *(uint64_t *) args[3];

    int rank;
    ABT_xstream_self_rank(&rank);

    int chunk_size = (to_exclusive + (num_threads - 1)) / num_threads;
    int func_from = chunk_size * rank;
    int func_to = chunk_size * (rank + 1);
    func_to = func_to > to_exclusive ? to_exclusive : func_to;
    func(func_from, func_to, 1, func_args);
}

void ompc_loop_divide_conquer(loop_func_t func, int nargs, void *args,
                              uint64_t from, uint64_t to_exclusive, int step, int num_tasks)
{
    // assume num_threads > 1
    ABT_task tasks[num_threads - 1];
    void *loop_schedule_args[] = { func, args, &from, &to_exclusive };
    for (int i = 0; i < num_threads - 1; i++) {
        ABT_task_create_on_xstream(xstreams[i + 1], loop_schedule, loop_schedule_args, &tasks[i]);
    }

    loop_schedule(loop_schedule_args);

    for (int i = 0; i < num_threads - 1; i++) {
        ABT_task_join(tasks[i]);
        ABT_task_free(&tasks[i]);
    }
}
