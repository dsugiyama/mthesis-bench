#include <stdio.h>
#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <omp.h>

int main(void)
{
    int bindings[256];
    int num_threads;

    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();

        cpu_set_t cpuset;
        pthread_t self = pthread_self();
        pthread_getaffinity_np(self, sizeof(cpu_set_t), &cpuset);

        int id = omp_get_thread_num();
        for (int i = 0; i < CPU_SETSIZE; i++) {
            if (CPU_ISSET(i, &cpuset)) {
                bindings[id] = i;
                break;
            }
        }
    }

    for (int i = 0; i < num_threads; i++) {
        printf("%d\n", bindings[i]);
    }
}
