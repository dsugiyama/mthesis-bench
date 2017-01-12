#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double get_clock(void);
int delay_length_from_sec(double delay_sec);
void delay(int delay_length);

int main(int argc, char *argv[])
{
    int NTHRDS = atoi(argv[1]);
    int DELAY_USEC = atoi(argv[2]);

    int delay_length = delay_length_from_sec(DELAY_USEC * 1E-6);

    omp_set_nested(1);

    #pragma omp parallel for num_threads(NTHRDS)
    for (int i = 0; i < NTHRDS; i++) {
        #pragma omp parallel for num_threads(NTHRDS)
        for (int j = 0; j < NTHRDS; j++) {
            delay(delay_length);  // simulated work
        }
    }

    double start = get_clock();

    #pragma omp parallel for num_threads(NTHRDS)
    for (int i = 0; i < NTHRDS; i++) {
        #pragma omp parallel for num_threads(NTHRDS)
        for (int j = 0; j < NTHRDS; j++) {
            delay(delay_length);  // simulated work
        }
    }

    double elapsed = get_clock() - start;
    printf("%f sec\n", elapsed);
}
