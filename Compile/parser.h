#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include "tokens.h"

void program(aToken_type tok);
void print_pm0(FILE* outFile);
void print_pm0_screen();
void print_symboltable();
int find_valid_symbol_kind(char* identstr, int kind);

#endif
