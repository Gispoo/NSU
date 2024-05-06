#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../huffman.h"
#include "../Errors/errors.h"

void size_file_and_num_zero(FILE* in, int* size_file, int* num_zero) {
    fseek(in, 0, SEEK_END);
    
    *size_file = ftell(in);
    if (*size_file == 1) 
        exit(0);

    fseek(in, (*size_file) - 1, SEEK_SET);
    if (!fread(num_zero, 1, 1, in)) 
        error_read_symbol();

    fseek(in, 1, SEEK_SET);
}

unsigned char read_bit(BitContext* ctx, int num) {
    if (ctx->index == 7)
        if (!fread(&(ctx->symbol), sizeof(ctx->symbol), 1, ctx->in)) 
            error_read_symbol();
    
    if (num == 1) {
        unsigned char bit = (ctx->symbol >> ctx->index) & 1;
        --ctx->index;
        if (ctx->index == -1) {
            ctx->index = 7;
            ctx->symbol = 0;
        }

        return bit;
    } else {
        unsigned char bits = ctx->symbol << (7 - ctx->index);
        if (ctx->index != 7) {
            if (!fread(&(ctx->symbol), sizeof(ctx->symbol), 1, ctx->in))
                error_read_symbol();
            
            bits += (ctx->symbol >> (ctx->index + 1));
        }

        return bits;
    }
}

HTree* decode_huffman_tree(BitContext* ctx) {
    char bit = read_bit(ctx, 1);
    HTree* t = (HTree*) malloc(sizeof(HTree));

    if (bit == 0) {
        t->left = decode_huffman_tree(ctx);
        t->right = decode_huffman_tree(ctx);
    } else {
        t->left = NULL;
        t->right = NULL;
        t->symbol = read_bit(ctx, 8);
    }

    return t;
}

HTree* decode_symbol(BitContext* ctx, HTree* root) {  
    if (is_leaf(root)) {
        fputc(root->symbol, ctx->out);
        return root;
    }
    if (ctx->index < 0) return root;

    int bit_value = (ctx->symbol >> ctx->index) & 1;
    --(ctx->index);

    HTree* t;
    if (bit_value)
        t = decode_symbol(ctx, root->right);
    else
        t = decode_symbol(ctx, root->left);

    return t;
}

void position_tree(BitContext* ctx, HTree* root, HTree* t) {
    if (t == NULL || is_leaf(t))
        t = decode_symbol(ctx, root);
    else
        t = decode_symbol(ctx, t);
    
    if (is_leaf(root)) 
        --ctx->index;
}

void decode(HTree* root, BitContext* ctx, unsigned char num_zero, int size_code) {
    ctx->index = 7;
    ctx->symbol = 0;

    if (!fread(&(ctx->symbol), sizeof(ctx->symbol), 1, ctx->in))
        error_read_symbol();
    
    HTree* t = NULL;
    for (int i = 1; i < size_code - 1; ++i) {
        while (ctx->index >= 0) {
            if (t == NULL || is_leaf(t))
                t = decode_symbol(ctx, root);
            else
                t = decode_symbol(ctx, t);
            
            if (is_leaf(root)) 
                --ctx->index;
        }

        fread(&(ctx->symbol), sizeof(ctx->symbol), 1, ctx->in);
        ctx->index = 7;
    }

    while (ctx->index + 1 != num_zero)
        position_tree(ctx, root, t);
}