#include <stdio.h>
#include <math.h>

int main() {
    unsigned long long n = 500000;

    int sg = -1;
    long double pi = 1;
    for (unsigned long long j = 1; j < n; ++j) {
        pi += sg / (2 * (long double)j + 1);
        sg *= -1;
    }

    long double x = 0.31134; 
    long double sinx = sin(x);

    printf("sin(x) = %.20Lf\n", sinx);
    printf("Pi = %.20Lf\n", 4 * pi);
}