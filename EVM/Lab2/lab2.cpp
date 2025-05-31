#include <iostream>
#include <windows.h>
#include <immintrin.h>
#include <intrin.h>

using namespace std;

int main() {
    for (unsigned long long i = 1; i <= 22; ++i) {
        unsigned long long start = __rdtsc();

        unsigned long long n = i * 500000000;

        int sg = -1;
        long double pi = 1;
        for (unsigned long long j = 1; j < n; ++j) {
            pi += sg / (2 * (long double)j + 1);
            sg *= -1;
        }

        //cout << " Pi = " << setprecision(10) << 4 * pi << endl;

        unsigned long long finish = __rdtsc();

        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        //long double frequency = 2100000000;

        cout << (long double) (finish - start) / frequency.QuadPart << endl;
    }
}