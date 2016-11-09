#ifndef TOKENS_H
#define TOKENS_H

typedef enum token {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

typedef struct {
  int col;
  int line;
  char* text;
  union {
    int number;
    char** ident;
  } val;
  token_type t;
} YYSTYPE;

token_type* tokArr;

#endif // TOKENS_H
