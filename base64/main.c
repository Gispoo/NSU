#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 3
#define MAX_SIZE2 1000000

enum {
    special_cod = 65,
    ignor = -1,
    error = -2
};

enum {
    four_arg = 4,
    five_arg = 5,
    six_arg = 6
};

typedef struct Input {
    char action;
    char option;
    int N;
} Input;

int input_data(int num_arg, char* argv[], Input* data) {
    switch (num_arg) {
    case four_arg:
        if (strcmp(argv[1], "-e") == 0) {
            data->action = 'e';
            data->option = NULL;
        } else if (strcmp(argv[1], "-d") == 0) {
            data->action = 'd';
            data->option = NULL;
        } else {
            return error;
        }
        break;
    case five_arg:
        if (strcmp(argv[1], "-i") == 0 && strcmp(argv[2], "-d") == 0) {
            data->action = 'd';
            data->option = ignor;
        } else {
            return error;
        }
        break;
    case six_arg:
        char *end;
        data->N = strtol(argv[3], &end, 10);
        if (strcmp(argv[1], "-e") == 0 && strcmp(argv[2], "-f") == 0 && *end == '\0') {
            data->action = 'e';
            data->option = 'f';
        } else {
            return error;
        }
        break;
    default:
        return error;
    }
    return 0;
}

void base64_encode(FILE *in, FILE *out, int option, int N) {
    char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    unsigned char bufer_in[MAX_SIZE], bufer_out[MAX_SIZE + 1];
    int x1 = 0b00000011, x2 = 0b00001111, x3 = 0b00111111;
    int read_bytes, index = 1;
    while (read_bytes = fread(bufer_in, 1, MAX_SIZE, in)) {
        bufer_out[0] = bufer_in[0] >> 2;
        bufer_out[1] = (read_bytes >= 2) ? ((bufer_in[0] & x1) << 4) + (bufer_in[1] >> 4) : (bufer_in[0] & x1) << 4;
        bufer_out[2] = (read_bytes >= 2) ? (read_bytes >= 3 ? ((bufer_in[1] & x2) << 2) + (bufer_in[2] >> 6) : (bufer_in[1] & x2) << 2) : special_cod;   
        bufer_out[3] = (read_bytes >= 3) ? bufer_in[2] & x3 : special_cod;
        
        for (int i = 0; i < MAX_SIZE + 1; ++i, ++index) {
            fputc(base64chars[bufer_out[i]], out);
            if ((option == 'f') && (index % N == 0)) {
                fputc('\n', out);
            }
        }
    }
}

int code_symbol_base64(unsigned char* symbol, int option) {
    char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    for (int i = 0; i < special_cod + 1; ++i) {
        if (*symbol == base64chars[i]) {
            *symbol = i;
            return 0;
        }
    }
    return (option == ignor) ? ignor : error;
}

void read_four_symbols(FILE* in, FILE* out, unsigned char bufer_in[], int option) {
    char error_str[] = "ERROR. Invalid encoded file.";
    for (int i = 0; i < MAX_SIZE + 1; ++i) {
        if ((bufer_in[i] = fgetc(in)) == EOF) {
            printf("%s\n1", error_str);
            exit(0);
        }
        switch (code_symbol_base64(&bufer_in[i], option)) {
        case error:
            printf("%s\n2", error_str);
            exit(0);
        case ignor:
            if (fgetc(in) == EOF) {
                printf("%s\n3", error_str);
                exit(0);
            }
            fseek(in, -1, SEEK_CUR);
            --i;
        }
    }
}

void base64_decode(FILE *in, FILE *out, int option) {
    unsigned char bufer_in[MAX_SIZE + 1], bufer_out[MAX_SIZE];
    int x1 = 0b001111, x2 = 0b000011;
    while (fgetc(in) != EOF) {
        fseek(in, -1, SEEK_CUR);
        read_four_symbols(in, out, bufer_in, option);
        int num_equally = 0;
        bufer_out[0] = (bufer_in[0] << 2) + (bufer_in[1] >> 4);
        bufer_out[1] = (bufer_in[2] == special_cod) ? ++num_equally : ((bufer_in[1] & x1) << 4) + (bufer_in[2] >> 2);
        bufer_out[2] = (bufer_in[3] == special_cod) ? ++num_equally : ((bufer_in[2] & x2) << 6) + bufer_in[3];

        for (int i = 0; i < MAX_SIZE && num_equally < 3; ++i, ++num_equally) {
            fputc(bufer_out[i], out);
        }
    }
}

int main(int argc, char* argv[]) {
    char help[] = "The parameters were entered incorrectly. Below is a list and description of the available function arguments:\n    -e infilename outfilename - encode an arbitrary file <infilename> to a text file <outfilename>.\n    -d infilename outfilename - decode a text file <infilename> in binary <outfilename>.\n    -i -d infilename outfilename - decode a text file <infilename> in binary <outfilename>. When decoding, ignore characters not from the Base64-alphabet.\n   -e -f N infilename outfilename - Encode the file <infilename> to a text file <outfilename>. Insert a newline every N characters in the output file.";
    FILE *in = fopen(argv[argc - 2], "rb");
    FILE *out = fopen(argv[argc - 1], "wb");
    if (in == NULL || out == NULL) {
        printf("%s\n", help);
        fclose(in);
        fclose(out);
        return 0;
    }

    Input data;
    if (input_data(argc, argv, &data) == error) {
        printf("%s\n", help);
    }

    if (data.action == 'e') {
        base64_encode(in, out, data.option, data.N);
    } else {
        base64_decode(in, out, data.option);
    }

    fclose(in);
    fclose(out);

    return 0;
}