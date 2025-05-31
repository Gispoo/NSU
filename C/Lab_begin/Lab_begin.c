#define _CTR_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

int main() {
    int N;
    long long a, sum = 0;
    if (scanf("%d", &N) != 1) {
        printf("Error");
        return 0;
    }


    if (N < 1 || N > 10) {
        printf("bad input");
        return 0;
    }

    for (int i = 0; i < N; ++i) {
        if (scanf("%lld", &a) != 1) {
            printf("bad input");
            return 0;
        }

        if (a <= INT_MAX && a >= INT_MIN) {
            sum += a;
        }
        else {
            printf("bad input");
            return 0;
        }        
    }


    if (sum > INT_MAX || sum < INT_MIN || scanf("%lld", &a) == 1) {
        printf("bad input");
    }
    else {
        printf("%lld", sum);
    }
    

    return 0;
}