#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>

#define MAX_SIZE 256

typedef struct HTree HTree;

struct HTree {
    unsigned char symbol;
    int freque;
    HTree* right;
    HTree* left;
};

typedef struct {
    unsigned char symbol;
    int index;
    FILE* in;
    FILE* out;
} BitContext;

void is_n_r_t(FILE* out, unsigned char symbol, int i, int ignor_symvol);

void print_code_table(FILE* out, unsigned char code_table[]);

void print_frequency_symbols(FILE* out, HTree leaves[], int num_symbols);

void print_tree(FILE* out, HTree* root);

void print_bits(unsigned char symbol);

void write_ctx(BitContext* ctx);

void read_ctx(BitContext* ctx);

void next_index(BitContext* ctx, char action);

BitContext* new_ctx(FILE* in, FILE* out, int symbol, int index);

int is_leaf(HTree* root);

int compare(const void *a, const void *b);

void free_tree(HTree* root);

void close_file(FILE* in, FILE* out);

void huffman_decode(FILE* in, FILE* out);

void huffman_encode(FILE* in, FILE* out);

#endif