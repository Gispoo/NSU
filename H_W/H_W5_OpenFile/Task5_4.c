#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int main() {
    char str[20], c;
    scanf("%s", &str);
   
    FILE *in = fopen("in.txt", "r");

    int n = 0, index;
    for (int i = 1; !feof(in); ++i) {
        fscanf(in, "%c", &c);
        if (n == strlen(str) + 2) {
            break;
        }
        else if ((str[n - 1] == c && (n != 0 && n != strlen(str) + 1)) || (c == ' ')) {
            if (n == 1) {
                index = i;
            }
            ++n;
        }
        else {
            n = 0;
        }
    }

    fclose(in);

    if (n == strlen(str) + 2) {
        printf("%d", index);
    }
    else {
        printf("%s not found in in.txt", str);
    }

    return 0;
}