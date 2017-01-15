#include <stdint.h>

int ompc_get_max_threads(void);
typedef void (*loop_func_t)(uint64_t from, uint64_t to_exclusive, int step, void *args[]);
void ompc_loop_divide_conquer(loop_func_t func, int nargs, void *args,
                              uint64_t from, uint64_t to_exclusive, int step, int num_tasks);
