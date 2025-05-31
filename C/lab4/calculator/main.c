#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_SIZE 1002
#define MAX_SIZE2 499
#define OPER 'o'
#define BRACKET 'b'
#define DIGIT 'd'
#define NUM_OPER 5
#define ERROR -1
#define TRUE 1

typedef struct StackInt {
    int data;
    struct StackInt* next;
} StackInt;

void push_int(StackInt** top, int data) {
    StackInt* p = (StackInt*) malloc(sizeof(StackInt));
    p->data = data;
    p->next = *top;
    *top = p;
}

int pop_int(StackInt** top) {
    if (*top == NULL) {
        printf("syntax error");
        return ERROR;
    } else {
        StackInt *p_next = (*top)->next;
        int result = (*top)->data;
        free(*top);
        *top = p_next;
        return result;
    }
}

typedef struct StackChar {
    char data;
    struct StackChar* next;
} StackChar;

void push_char(StackChar** top, char data) {
    StackChar* p = (StackChar*) malloc(sizeof(StackChar));
    p->data = data;
    p->next = *top;
    *top = p;
}

char pop_char(StackChar** top) {
    if (top == NULL) {
        printf("syntax error");
        return ERROR;
    } else {
        StackChar *p_next = (*top)->next;
        char result = (*top)->data;
        free(*top);
        *top = p_next;
        return result;
    }
}

typedef struct Operation {
    char operation;
    int priority;
} Operation;

void free_stack(StackInt** top_digit, StackChar** top_oper) {
    while (*top_digit != NULL) {
        StackInt* temp = *top_digit;
        *top_digit = (*top_digit)->next;
        free(temp);
    }
    while (*top_oper != NULL) {
        StackChar* temp = *top_oper;
        *top_oper = (*top_oper)->next;
        free(temp);
    }
}

char next_sumbol(char* str, int* index, int* result) {
    if (str[*index] == '(' || str[*index] == ')') {
        *result = str[*index];
        return str[*index];
    } else if (isdigit(str[*index]) == 0) {
        *result = str[*index];
        return OPER;
    } else {
        int num = str[*index] - '0';
        while (isdigit(str[*index + 1]) != 0) {
            num = num * 10 + (str[*index + 1] - '0');
            ++*index;
        }
        *result = num;
        return DIGIT;
    }
}

int prioritet(char operation) {
    Operation priority_operation[] = {
        {.operation = '+', .priority = 1},
        {.operation = '-', .priority = 1},
        {.operation = '*', .priority = 2},
        {.operation = '/', .priority = 2},
        {.operation = '(', .priority = 0},
    };
    for (int i = 0; i < NUM_OPER; ++i) {
        if (priority_operation[i].operation == operation) {
            return priority_operation[i].priority;
        }
    }
    printf("syntax error");
    return ERROR;
}

void action(StackInt** top_digit, StackChar** top_operation) {
    int b = pop_int(top_digit), a = pop_int(top_digit);
    char oper = pop_char(top_operation);
    if (b == ERROR || a == ERROR || oper == ERROR) {
        free_stack(top_digit, top_operation);
        exit(0);
    }
    switch (oper) {
        case '+':
            push_int(top_digit, a + b);
            break;
        case '-':
            push_int(top_digit, a - b);
            break;
        case '*':
            push_int(top_digit, a * b);
            break;
        case '/':
            if (b == 0) {
                printf("division by zero");
                free_stack(top_digit, top_operation);
                exit(0);
            }
            push_int(top_digit, a / b);
            break;
    }
}

void oper(int symbol, StackInt** top_digit, StackChar** top_operation) {
    int priority = prioritet((char)symbol);
    if (priority == ERROR) {
        free_stack(top_digit, top_operation);
        exit(0);
    }
    while (TRUE) {
        if ((*top_operation) == NULL || priority > prioritet((*top_operation)->data)) {
            push_char(top_operation, (char)symbol);
            break;
        } else {
            action(top_digit, top_operation);
        }
    }
}

void bracket_open(int symbol, int* num_open_bracket, StackInt** top_digit, StackChar** top_operation) {
    push_char(top_operation, (char)symbol);
    push_int(top_digit, symbol);
    ++(*num_open_bracket);
    if (*num_open_bracket >= MAX_SIZE2) {
        printf("syntax error");
        free_stack(top_digit, top_operation);
        exit(0);
    }
}

void bracket_close(int* num_open_bracket, int* num_close_bracket, StackInt** top_digit, StackChar** top_operation) {
    ++(*num_close_bracket);
    if ((*num_open_bracket) < (*num_close_bracket) || (*top_digit)->data == '(') {
        printf("syntax error");
        free_stack(top_digit, top_operation);
        exit(0);
    }
    while ((*top_operation)->data != '(') {
        action(top_digit, top_operation);
    }
    pop_char(top_operation);
    int a = pop_int(top_digit);
    (*top_digit)->data = a;
    --(*num_open_bracket);
    --(*num_close_bracket);
}

void read(char* str, StackInt** top_digit, StackChar** top_operation) {
    int len_str = strlen(str) - 1, num_open_bracket = 0, num_close_bracket = 0;
    for (int i = 0; i < len_str; ++i) {
        int symbol;
        switch (next_sumbol(str, &i, &symbol)) {
        case OPER: { 
            oper(symbol, top_digit, top_operation);
            break;
        } case '(': {
            bracket_open(symbol, &num_open_bracket, top_digit, top_operation);
            break;
        } case ')': {
            bracket_close(&num_open_bracket, &num_close_bracket, top_digit, top_operation);
            break;
        } case DIGIT:
            push_int(top_digit, symbol);
            break;
        }
    }
}

int calculator(char* str) {
    StackInt* top_digit = NULL;
    StackChar* top_operation = NULL;
    read(str, &top_digit, &top_operation);
    if (top_digit == NULL) {
        printf("syntax error");
        free_stack(&top_digit, &top_operation);
        exit(0);
    }
    while (!(top_digit->next == NULL && top_operation == NULL)) {
        action(&top_digit, &top_operation);
    }
    int result = top_digit->data;
    free_stack(&top_digit, &top_operation);
    return result;
}

int main() {
    char str[MAX_SIZE];
    if (fgets(str, MAX_SIZE, stdin) == NULL) {
        return ERROR;
    }

    printf("%d", calculator(str));

    return 0;
}