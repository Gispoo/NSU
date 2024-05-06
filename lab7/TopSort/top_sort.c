#include "top_sort.h"
#include "../Errors/errors.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 2000

typedef struct {
    int top;
    int vertic[MAX_SIZE];
} Stack;

void push(Stack* stack, int v) {
    ++(stack->top);
    stack->vertic[stack->top] = v;
}

int pop(Stack* stack) {
    return stack->vertic[--(stack->top)];
}

void dfs(FILE* in, FILE* out, Stack* stack, char graf[], char mass[], int v, int num_vertic) {
    mass[v] = 1;
    
    for (int i = 0; i < num_vertic; ++i) {
        if (graf[v * num_vertic + i] == 1 && mass[i] == 1)
            impossible_sort(in, out);
            
        if (graf[v * num_vertic + i] == 1 && mass[i] == 0)
            dfs(in, out, stack, graf, mass, i, num_vertic);
    }

    mass[v] = 2;
    push(stack, v + 1);
}

void enter(FILE* in, FILE* out, int num_vertic, int num_edg, char graf[]) {
    if (num_vertic < 0 || num_vertic > MAX_SIZE)
        bad_number_of_vertices(in, out);

    if (num_edg < 0 || num_edg > num_vertic * (num_vertic - 1) / 2)
        bad_number_of_edges(in, out);    

    for (int i = 0; i < num_edg; ++i) {
        int a, b;
        if (fscanf(in, "%d %d", &a, &b) != 2)
            bad_number_of_lines(in, out);

        if (a < 1 || a > num_vertic || b < 1 || b > num_vertic)
            bad_vertex(in, out);

        graf[(a - 1) * num_vertic + (b - 1)] = 1;
    }
}

void top_sort(FILE* in, FILE* out) {
    int num_vertic, num_edg;
    if (fscanf(in, "%d %d", &num_vertic, &num_edg) != 2)
        bad_number_of_lines(in, out);

    char* graf = (char*) malloc(num_vertic * num_vertic * sizeof(char));

    enter(in, out, num_vertic, num_edg, graf);

    char* mass = (char*) malloc(num_vertic * sizeof(char));
    for (int i = 0; i < num_vertic; ++i)
        mass[i] = 0;
    
    Stack stack = {-1, {0}};
    for (int i = 0; i < num_vertic; ++i)
        if (!mass[i]) dfs(in, out, &stack, graf, mass, i, num_vertic);

    if (stack.top != num_vertic - 1) {
        impossible_sort(in, out);
    } else {
        while (stack.top != -1) {
            fprintf(out, "%d ", stack.vertic[stack.top]);
            --(stack.top);
        }
    }
    
    free(graf);
    free(mass);
}