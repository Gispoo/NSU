#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Encode/encode.h"
#include "Decode/decode.h"
#include "Errors/errors.h"
#include "huffman.h"

int main() {
    FILE* in = fopen("in.txt", "rb");
    if (in == NULL) error_open_file();
    FILE* out = fopen("out.txt", "wb");
    if (out == NULL) error_open_file();

    char action;
    if (fscanf(in, "%c", &action) != 1) {
        close_file(in, out);

        error_read_symbol();
    }

    if (action == 'c')
        huffman_encode(in, out);
    else if (action == 'd')
        huffman_decode(in, out);
    else
        printf("Error action.\n");
    
    close_file(in, out);

    return 0;
}