#include <bits\stdc++.h>
#include <intrin.h>

using namespace std;

int main() {
    unsigned long long start = __rdtsc();

    long long n = 3000000000;

    int sg = -1;
    long double pi = 1;
    for (long long i = 1; i < n; ++i) {
        pi += sg / (2 * (long double)i + 1);
        sg *= -1;
    }

    cout << setprecision(10) << 4 * pi << endl;

    unsigned long long finish = __rdtsc();

    long double frequency = 2100000000;

    cout << finish - start << endl << (long double) (finish - start) / frequency;
}