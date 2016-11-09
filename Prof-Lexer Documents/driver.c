#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "tokens.h"

extern YYSTYPE yylval;
extern FILE *yyin;

#ifndef LEXER
#define LEXER

int do_lex(FILE* clean) {

  int tok;
  /*
  if (argc != 2) {
    fprintf(stderr, "usage: ./lextest filename\n");
    exit(1);
  }*/

  yyin = clean;
  if (!yyin) {
    printf("cannot file open file");
    exit(1);
  }

  for(;;) {
    tok = yylex();
    if (tok == 0) break;
    printf("(%2d,%2d) %-12s %d", yylval.line, yylval.col, yylval.text, tok);
    yylval.t = tok;
    switch (tok) {
    case identsym :
      printf("    %s\n", yylval.val.ident);
      break;
    case numbersym :
      printf("    %d\n", yylval.val.number);
      break;
    default :
      printf("\n");
    }
  }
  return 0;
}

#endif // LEXER
