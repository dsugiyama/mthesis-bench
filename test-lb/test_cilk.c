#include <stdio.h>
#include <cilk/cilk.h>

double get_clock(void);
int delay_length_from_sec(double delay_sec);
void delay(int delay_length);

int delay_length;
double iter_start[256];

int main(int argc, char *argv[])
{
    delay_length = delay_length_from_sec(0.001);

    #pragma cilk grainsize = 1
    cilk_for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 100; j++) {
            delay(delay_length);
        }
    }

    double loop_start = get_clock();
    #pragma cilk grainsize = 1
    cilk_for (int i = 0; i < 256; i++) {
        iter_start[i] = get_clock();
        for (int j = 0; j < 100; j++) {
            delay(delay_length);
        }
    }

    for (int i = 0; i < 256; i++) {
        printf("%e\n", iter_start[i] - loop_start);
    }

    return 0;
}
