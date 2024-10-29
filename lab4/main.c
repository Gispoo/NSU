//#include <stdio.h>

int main() {
    long long n = 3000000000;

    int sg = -1;
    long double pi = 1;
    for (long long i = 1; i < n; ++i) {
        pi += sg / (2 * (long double)i + 1);
        sg *= -1;
    }

    return 0;
}