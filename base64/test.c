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
  //char base64chars[] = "          1         2         3         4         5         6    ";
  //char base64chars[] = "01234567890123456789012345678901234567890123456789012345678901234";
    char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    for (int i = 0; i < special_cod; ++i) {
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
            fprintf(out, "%s\n1", error_str);
            exit(0);
        }
        switch (code_symbol_base64(&bufer_in[i], option)) {
        case error:
            fprintf(out, "%s\n2", error_str);
            exit(0);
        case ignor:
            if (fgetc(in) == EOF) {
                fprintf(out, "%s\n3", error_str);
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

int main() {
    FILE *in = fopen("in.txt", "rb");
    FILE *out = fopen("out.txt", "wb");
    if (in == NULL || out == NULL) {
        printf("Error file");
        return 0;
    }

    base64_decode(in, out, ignor);

    fclose(in);
    fclose(out);

    return 0;
}
