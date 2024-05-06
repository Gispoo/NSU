#pragma once

#include <stdio.h>

void error_open_file();

void error_read(FILE* in, FILE* out);

void bad_number_of_vertices(FILE* in, FILE* out);

void bad_number_of_edges(FILE* in, FILE* out);

void bad_vertex(FILE* in, FILE* out);

void bad_number_of_lines(FILE* in, FILE* out);

void impossible_sort(FILE* in, FILE* out);
