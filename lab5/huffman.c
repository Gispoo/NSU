#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "Encode/encode.h"
#include "Decode/decode.h"
#include "Errors/errors.h"

void is_n_r_t(FILE* out, unsigned char symbol, int i, int ignor_symvol) {
    if (symbol != ignor_symvol && i != '\n' && i != '\r')
        fprintf(out, "\n");

    if (symbol != ignor_symvol && i != '\t')
        fprintf(out, "%c - ", i);

    else if (symbol != ignor_symvol)
        fprintf(out, "%c-", i);
}

void print_code_table(FILE* out, unsigned char code_table[]) {
    for (int i = 0; i < MAX_SIZE; ++i) {
        is_n_r_t(out, code_table[i * MAX_SIZE], i, 0);
        
        for (int j = 0; code_table[i * MAX_SIZE + j] != '\0' && j < MAX_SIZE; ++j) {
            fprintf(out, "%c", code_table[i * MAX_SIZE + j]);
        }
    }
}

void print_frequency_symbols(FILE* out, HTree leaves[], int num_symbols) {
    int sum = 0;
    for (int i = 0; i < num_symbols; ++i) {
        if (leaves[i].freque != 0) {
            sum += leaves[i].freque;
            is_n_r_t(out, leaves[i].symbol, i, -1);
            fprintf(out, "%d", leaves[i].freque);
        }
    }
    fprintf(out, "\nsum = %d\n--------------\n", sum);
}

void print_tree(FILE* out, HTree* root) {
    if (is_leaf(root)) {
        fprintf(out, "%c; ", root->symbol);
        return;
    }
    fprintf(out, "*<- ");
    print_tree(out, root->left);
    fprintf(out, "-> ");
    print_tree(out, root->right);
}

void print_bits(unsigned char symbol) {
    printf("%d ", symbol);
    for (int i = 7; i >= 0; --i) {
        printf("%d", (symbol >> i) & 1);
        if (i % 8 == 4)
            printf(" ");
    }
    printf(", ");
}

void write_ctx(BitContext* ctx) {
    //print_bits(ctx->symbol);
    fwrite(&(ctx->symbol), sizeof(ctx->symbol), 1, ctx->out);
    ctx->symbol = 0;
    ctx->index = 7;
}

void read_ctx(BitContext* ctx) {
    if (!fread(&(ctx->symbol), sizeof(ctx->symbol), 1, ctx->in)) 
        error_read_symbol();
    ctx->index = 7;
}

void next_index(BitContext* ctx, char action) {
    --ctx->index;
    if (ctx->index == -1) {
        if (action == 'w')
            write_ctx(ctx);
        else if (action == 'r')
            read_ctx(ctx);
        else
            error_action_in_next_index();
    }
}

BitContext* new_ctx(FILE* in, FILE* out, int symbol, int index) {
    BitContext* ctx = (BitContext*) malloc(sizeof(BitContext));
    ctx->symbol = symbol;
    ctx->index = index;
    ctx->in = in;
    ctx->out = out;
    return ctx;
}

int is_leaf(HTree *root) {
    return (root->right == NULL && root->left == NULL);
}

int compare(const void *a, const void *b) {
    HTree* htreeA = (HTree*)a;
    HTree* htreeB = (HTree*)b;
    if (htreeA->freque == 0 && htreeB->freque != 0)
        return 1;

    else if (htreeA->freque != 0 && htreeB->freque == 0)
        return -1;
        
    else
        return htreeA->freque - htreeB->freque;

    return htreeA->freque - htreeB->freque;
}

void free_tree(HTree* root) {
    if (is_leaf(root)) {
        free(root);
        return;
    }
    HTree* left = root->left;
    HTree* right = root->right;
    free_tree(left);
    free_tree(right);
    free(root);
}

void close_file(FILE* in, FILE* out) {
    fclose(in);
    fclose(out);
    exit(0);
}

void huffman_decode(FILE* in, FILE* out) {
    int num_zero, size_file;
    size_file_and_num_zero(in, &size_file, &num_zero);

    BitContext* ctx = new_ctx(in, out, 0, 7);
    HTree* root = decode_huffman_tree(ctx);
    if (root == NULL) exit(0);

    int size_tree = ftell(in);
    decode(root, ctx, num_zero, size_file - size_tree);

    free(ctx);
    free_tree(root);
}

void huffman_encode(FILE* in, FILE* out) {
    int alphabet[MAX_SIZE] = {0};
    frequency(in, alphabet);

    HTree leaves[2 * MAX_SIZE - 1];
    int num_symbols = 0;
    for (int i = 0; i < MAX_SIZE; ++i) {
        leaves[i].freque = 0;
        new_leaf(leaves, &num_symbols, alphabet[i], i);
    }

    qsort(leaves, MAX_SIZE, sizeof(HTree), compare);
    HTree* root;
    char code_table[MAX_SIZE * MAX_SIZE] = {'\0'}, code[MAX_SIZE] = {'\0'};
    if (num_symbols == 1) {
        root = (HTree*) malloc(sizeof(HTree));
        root->freque = leaves[0].freque;
        root->left = NULL;
        root->right = NULL;
        root->symbol = leaves[0].symbol;
        code[0] = '0';
        build_code_table(root, code_table, code, 1);
    } else {
        root = creatTree(leaves, num_symbols);
        if (root == NULL) exit(0);
        build_code_table(root, code_table, code, 0);
    }

    BitContext* ctx = new_ctx(in, out, 0, 7);
    title_write(root, ctx);
    if (ctx->index != 7) write_ctx(ctx);

    encode(code_table, ctx);

    free(ctx);
    free(root);
}