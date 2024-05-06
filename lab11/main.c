#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define MAX_SIZE 10000

typedef struct {
    int weight;
    int value;
} Item;

int max(int a, int b) {
    return (a > b) ? a : b;
}

void backpack(int n, int w, Item items[]) {
    int dp[n + 1][w + 1];

    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= w; ++j) {
            if (i == 0 || j == 0) {
                dp[i][j] = 0;
            } else if (items[i - 1].weight <= j) {
                dp[i][j] = max(items[i - 1].value + dp[i - 1][j - items[i - 1].weight], dp[i - 1][j]);
            } else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    printf("%d\n", dp[n][w]);
    Item final_item[MAX_SIZE];
    int k = 0;
    for (int i = n, j = w; i > 0 && j > 0; --i) {
        if (dp[i][j] != dp[i - 1][j]) {
            final_item[k] = items[i - 1];
            j -= items[i - 1].weight;
            ++k;
        }
    }

    for (int i = k - 1; i >= 0; --i) {
        printf("%d %d\n", final_item[i].weight, final_item[i].value);
    }
}

int main() {
    int n, w;
    if (scanf("%d %d", &n, &w) != 2) {
        return 0;
    }
    
    Item items[n];
    for (int i = 0; i < n; ++i) {
        if (scanf("%d %d", &items[i].weight, &items[i].value) != 2) {
            return 0;
        }
    }

    backpack(n, w, items);

    return 0;
}
