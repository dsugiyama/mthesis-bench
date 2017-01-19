#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include "ompc.h"

typedef void (*loop_func_t)(uint64_t from, uint64_t to_exclusive, int step, void *args[]);
int ompc_main(int argc, char *argv[]);

static int num_threads = 1;
static pthread_t threads[256];
_Thread_local int tid;

bool loop_start[256];
void *loop_info;
bool volatile worker_end = false;

pthread_barrier_t barrier;

static void loop_schedule(void *args[])
{
    loop_func_t func = args[0];
    void *func_args = args[1];
    // assume from == 0
    uint64_t to_exclusive = *(uint64_t *) args[3];

    int chunk_size = (to_exclusive + (num_threads - 1)) / num_threads;
    int func_from = chunk_size * tid;
    int func_to = chunk_size * (tid + 1);
    func_to = func_to > to_exclusive ? to_exclusive : func_to;
    func(func_from, func_to, 1, func_args);
    
    pthread_barrier_wait(&barrier);
}

void *worker_main(void *arg)
{
    tid = (int) arg;

    while (true) {
        while (!((bool volatile *) loop_start)[tid]) ;
        __sync_synchronize();
        loop_start[tid] = false;
        if (worker_end) return NULL;
        loop_schedule(loop_info);
    }
}

int main(int argc, char *argv[])
{
    char *num_threads_env = getenv("OMPC_NUM_PROCS");
    if (num_threads_env != NULL) {
        num_threads = atoi(num_threads_env);
    }

    pthread_barrier_init(&barrier, NULL, num_threads);
    for (int i = 0; i < num_threads; i++) {
        loop_start[i] = false;
    }

    threads[0] = pthread_self();
    tid = 0;
    for (int i = 1; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker_main, (void *) i);
    }

    ompc_main(argc, argv);

    worker_end = true;
    for (int i = 1; i < num_threads; i++) {
        loop_start[i] = true;
        void *retval;
        pthread_join(threads[i], &retval);
    }
}

int ompc_get_max_threads(void)
{
    return num_threads;
}

void ompc_loop_divide_conquer(loop_func_t func, int nargs, void *args,
                              uint64_t from, uint64_t to_exclusive, int step, int num_tasks)
{
    // assume num_threads > 1
    void *loop_schedule_args[] = { func, args, &from, &to_exclusive };
    loop_info = loop_schedule_args;
    __sync_synchronize();
    for (int i = 1; i < num_threads; i++) {
        loop_start[i] = true;
    }

    loop_schedule(loop_schedule_args);
}
