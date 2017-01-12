#include <stdio.h>

double get_clock(void);

void delay(int delay_length)
{
    float a = 0.0;

    for (int i = 0; i < delay_length; i++) {
        a += i;
    }

    if (a < 0) {
        printf("%f\n", a);
    }
}

int delay_length_from_sec(double delay_sec)
{
    int reps = 100;
    int delay_length = 0;
    double lapsed_sec = 0.0;

    delay(delay_length);

    while (lapsed_sec < delay_sec) {
        delay_length = delay_length * 1.1 + 1;
        double start = get_clock();
        for (int i = 0; i < reps; i++) {
            delay(delay_length);
        }
        lapsed_sec = (get_clock() - start) / reps;
    }

    return delay_length;
}
