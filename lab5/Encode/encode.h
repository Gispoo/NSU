#ifndef ENCODE_H
#define ENCODE_H

#include <stdio.h>
#include "../huffman.h"

void new_leaf(HTree leaves[], int* num_symbols, int i, unsigned char symbol);

void frequency(FILE* in, int alphabet[]);

void insert_vertex(HTree leaves[], HTree* prt, int num_symbols, int index);

HTree* creatTree(HTree leaves[], int num_symbols);

void build_code_table(HTree* root, char code_table[], char code[], int code_lenght);

void title_write(HTree* root, BitContext* ctx);

void encode_byte(BitContext* ctx, unsigned char code_table[], unsigned char symbol);

void encode(unsigned char code_table[], BitContext* ctx);

#endif