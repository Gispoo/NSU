#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../huffman.h"

void frequency(FILE* in, int alphabet[]) {
    unsigned char symbol;
    while (fread(&symbol, sizeof(symbol), 1, in)) {
        ++alphabet[(int)symbol];
    }
    fseek(in, 1, SEEK_SET);
}

void new_leaf(HTree leaves[], int* num_symbols, int freque, unsigned char symbol) {
    leaves[*num_symbols].symbol = symbol;
    leaves[*num_symbols].freque = freque;
    leaves[*num_symbols].left = NULL;
    leaves[*num_symbols].right = NULL;
    if (freque != 0) ++(*num_symbols);
}

void insert_vertex(HTree leaves[], HTree* prt, int num_vertice, int index) {
    for (int i = index; ; ++i) {
        if (i < num_vertice - 1 && prt->freque <= leaves[i].freque) {
            for (int j = num_vertice - 1; j > i; --j) {
                leaves[j] = leaves[j - 1];
            }
            leaves[i] = *prt;
            break;
        } else if (i > num_vertice - 2) {
            leaves[i] = *prt;
            break;
        }
    }
    if (index != num_vertice - 1) free(prt);
}

HTree* creatTree(HTree leaves[], int num_symbols) {
    HTree* prt = NULL;
    for (int i = 0; i < 2 * num_symbols - 2; ) {
        prt = (HTree*) malloc(sizeof(HTree));
        prt->freque = leaves[i].freque + leaves[i + 1].freque;
        prt->left = &(leaves[i]);
        prt->right = &(leaves[i + 1]);
        i += 2;
        insert_vertex(leaves, prt, num_symbols + i / 2, i);
    }
    return prt;
}

void build_code_table(HTree* root, char code_table[], char code[], int code_lenght) {
    if (is_leaf(root)) {
        strncpy(&code_table[(root->symbol) * MAX_SIZE], code, code_lenght);
        return;
    }
    code[code_lenght] = '0';
    build_code_table(root->left, code_table, code, code_lenght + 1);
    code[code_lenght] = '1';
    build_code_table(root->right, code_table, code, code_lenght + 1);
}

void title_write(HTree* root, BitContext* ctx) {
    if (is_leaf(root)) {
        ctx->symbol |= (1 << ctx->index);
        --ctx->index;
        if (ctx->index == -1) write_ctx(ctx);
        for (int i = 7; i >= 0; --i) {
            ctx->symbol |= ((root->symbol >> i) & 1) << ctx->index;
            --ctx->index;
            if (ctx->index == -1) write_ctx(ctx);
        }
        return;
    }
    --ctx->index;
    if (ctx->index == -1) write_ctx(ctx);
    title_write(root->left, ctx);
    title_write(root->right, ctx);
}

void encode_byte(BitContext* ctx, unsigned char code_table[], unsigned char symbol) {
    for (int i = 0; code_table[symbol * MAX_SIZE + i] != '\0'; ++i) {
        if (code_table[symbol * MAX_SIZE + i] == '1')
            ctx->symbol |= (1 << ctx->index);
        
        --(ctx->index);
        if (ctx->index == -1) write_ctx(ctx);
    }
}

void encode(unsigned char code_table[], BitContext* ctx) {
    unsigned char symbol;
    while (fread(&symbol, sizeof(symbol), 1, ctx->in)) {
        encode_byte(ctx, code_table, symbol);
    }
    unsigned char num_zero = (ctx->index + 1) % 8;
    if (ctx->index != 7)
        write_ctx(ctx);
    
    fwrite(&num_zero, sizeof(num_zero), 1, ctx->out);
}