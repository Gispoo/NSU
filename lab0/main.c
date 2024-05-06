#define _CRT_SECURE_NO_WARNINGS
#define MAX_SIZE 15
#define MAX_SIZE2 48
#define FAIL -1
#include <stdio.h>
#include <string.h>

long double powd(int a, int b) {
    long double s = 1;
    if (b < 0) {
        for (int i = 0; i > b; --i) {
            s /= a;
        }       
    }
    else if (b == 0) {
        return 1;
    }

    return s;
}

long long powi(int a, int b) {
    long long s = 1;
    if (b > 0) {
        for (int i = 0; i < b; ++i) {
            s *= a;
        }
    }
    else  if (b == 0) {
        return 1;
    }

    return s;
}

int int_symbol(char c) {
    if (c >= 'a') {
        return c - ('a' - 10);
    }
    else {
        return c - '0';
    }
}

void integ_trans(char *arr_int_b1, char *arr_int_b2, int b1, int b2) {
    long long s = 0;
    int len_intag = strlen(arr_int_b1);
    for (int i = 0; i < len_intag; ++i) {
        s += int_symbol(arr_int_b1[i]) * powi(b1, strlen(arr_int_b1) - i - 1);
    }
    
    arr_int_b2[0] = '0';
    for (int i = 0; i < MAX_SIZE2 && s > 0; ++i) {
        if (s % b2 < 10) {
            arr_int_b2[i] = s % b2 + '0';
        }
        else {
            arr_int_b2[i] = s % b2 + ('a' - 10);
        }

        s /= b2;
    }
}

void frac_trans(char *arr_frac_b1, char *arr_frac_b2, int b1, int b2) {
    long double s = 0;
    int len_frac = strlen(arr_frac_b1);
    for (int i = 0; i < len_frac; ++i) {
        s += int_symbol(arr_frac_b1[i]) * powd(b1, -1 - i);
    }

    arr_frac_b2[0] = '.';
    for (int i = 1; i < MAX_SIZE && s > 0; ++i) {
        int whole = s * b2;

        if (whole < 10) {
            arr_frac_b2[i] = whole + '0';
        }
        else {
            arr_frac_b2[i] = whole + ('a' - 10);
        }

        s = s * b2 - whole;
    }
}

int check_cond_syst(int b) {
    if (b > 1 && b < 17) {
        return 0;
    }
    else {
        printf("bad input");
        return FAIL;
    }
}

int check_cond_num_and_build_integ_frac(char *arr, int b1, char *arr_int, char *arr_frac) {
    char s[] = ".0123456789abcdef";
    int dot = 0, index_dot, num_cor_symb = 0, check_0 = 0, len_arr = strlen(arr);

    for (int i = 0; i < len_arr; ++i) {
        if (arr[0] == '.' || arr[len_arr - 1] == '.' || (arr[i] == '.' && dot != 0)) {
            printf("bad input");
            return FAIL;
        }

        if (arr[i] >= 'A' && arr[i] <= 'Z') {
            arr[i] += 'a' - 'A';
        }

        for (int j = 0; j <= b1; ++j) {
            if (arr[i] == s[j]) {
                if (arr[i] != '0' && dot > 0) {
                    check_0 = 1;
                }

                if (arr[i] == '.') {
                    index_dot = i;
                    ++dot;
                }

                ++num_cor_symb;
            }
        }

        if (dot == 0) {
            arr_int[i] = arr[i];
        }
        else if (arr[i] != '.') {
            arr_frac[i - index_dot - 1] = arr[i];
        }

    }
    
    if (num_cor_symb == len_arr) {
        return check_0;
    }
    else {
        printf("bad input");
        return FAIL;
    }
}

void conclusion(char *arr_integ, char *arr_frac, int check_0) {
    int len_integ = strlen(arr_integ);
    for (int i = len_integ - 1; i >= 0; --i) {
        printf("%c", arr_integ[i]);
    }
    
    if (check_0 == 1) {
        for (int i = 0; i < MAX_SIZE - 2; ++i) {
            if (arr_frac[i] == '\0') {
                printf("0");
            }
            else {
                printf("%c", arr_frac[i]);
            }    
        }
    }  
}

int main() {
    int b1, b2;
    if (scanf("%d%d", &b1, &b2) != 2) {
        return 0;
    }

    if (check_cond_syst(b1) == FAIL || check_cond_syst(b2) == FAIL) {
        return 0;
    }

    char arr[MAX_SIZE + 1];
    if (scanf("%13s", arr) != 1) {
        return 0;
    }

    int check_0;
    char arr_int_b1[MAX_SIZE2] = {'\0'}, arr_frac_b1[MAX_SIZE] = {'\0'};
    if ((check_0 = check_cond_num_and_build_integ_frac(arr, b1, arr_int_b1, arr_frac_b1)) == FAIL) {
        return 0;
    }

    char arr_int_b2[MAX_SIZE2] = {'\0'};
    integ_trans(arr_int_b1, arr_int_b2, b1, b2);

    char arr_frac_b2[MAX_SIZE + 1] = {'\0'};
    if (check_0 == 1) {
        frac_trans(arr_frac_b1, arr_frac_b2, b1, b2);
    }

    conclusion(arr_int_b2, arr_frac_b2, check_0);

    return 0;
}