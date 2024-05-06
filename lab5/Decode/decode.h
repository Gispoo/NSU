#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "../huffman.h"

void size_file_and_num_zero(FILE* in, int* size_file, int* num_zero);

char read_bit(BitContext* ctx, int num);

HTree* decode_huffman_tree(BitContext* ctx);

HTree* decode_symbol(BitContext* ctx, HTree* root);

void decode(HTree* root, BitContext* ctx, unsigned char num_zero, int size_file);

#endif