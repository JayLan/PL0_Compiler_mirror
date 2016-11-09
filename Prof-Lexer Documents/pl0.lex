%{
#include <string.h>
#include "util.h"
#include "tokens.h"

static int LEX_line     = 1; 
static int LEX_col      = 1;
static int LEX_leng     = 0;

YYSTYPE yylval;

int yywrap(void) {
 return 1;
}

void newline(void) {
  LEX_line++;
  LEX_col  = 1;
  LEX_leng = 0;
}

void adjust(void) {
  LEX_col += LEX_leng;
  LEX_leng = yyleng;
  // update yylval
  yylval.col  = LEX_col;
  yylval.line = LEX_line;
  yylval.text = String(yytext);
}

void show_error_msg(string msg) {
  printf("(%2d,%2d) %s <-- LEX ERROR: %s\n", yylval.line, yylval.col, yylval.text, msg);
}

%}

ident           [a-zA-Z][a-zA-Z0-9]*
ident_too_long  [a-zA-Z][a-zA-Z0-9]{12,}
number          [0-9]{1,5}
number_too_long [0-9]{6,}

%Start COMMENT
%%
<INITIAL>"/*"              {adjust(); BEGIN COMMENT;}
<COMMENT>"*/"              {adjust(); BEGIN INITIAL;}
<COMMENT>.                 {adjust();}

<INITIAL>\n                {newline(); continue;}
<INITIAL>" "               {adjust(); continue;}
<INITIAL>"+"               {adjust(); return plussym;}
<INITIAL>"-"               {adjust(); return minussym;}
<INITIAL>"*"               {adjust(); return multsym;}
<INITIAL>"/"               {adjust(); return slashsym;}
<INITIAL>"odd"             {adjust(); return oddsym;}
<INITIAL>"="               {adjust(); return eqsym;}
<INITIAL>"<>"              {adjust(); return neqsym;}
<INITIAL>"<="              {adjust(); return leqsym;}
<INITIAL>"<"               {adjust(); return lessym;}
<INITIAL>">="              {adjust(); return geqsym;}
<INITIAL>">"               {adjust(); return gtrsym;}
<INITIAL>"("               {adjust(); return lparentsym;}
<INITIAL>")"               {adjust(); return rparentsym;}
<INITIAL>","               {adjust(); return commasym;}
<INITIAL>";"               {adjust(); return semicolonsym;}
<INITIAL>"."               {adjust(); return periodsym;}
<INITIAL>":="              {adjust(); return becomessym;}
<INITIAL>"begin"           {adjust(); return beginsym;}
<INITIAL>"end"             {adjust(); return endsym;}
<INITIAL>"if"              {adjust(); return ifsym;}
<INITIAL>"then"            {adjust(); return thensym;}
<INITIAL>"while"           {adjust(); return whilesym;}
<INITIAL>"do"              {adjust(); return dosym;}
<INITIAL>"call"            {adjust(); return callsym;}
<INITIAL>"const"           {adjust(); return constsym;}
<INITIAL>"var"             {adjust(); return varsym;}
<INITIAL>"procedure"       {adjust(); return procsym;}
<INITIAL>"write"           {adjust(); return writesym;}
<INITIAL>"read"            {adjust(); return readsym;}
<INITIAL>{number_too_long} {adjust(); show_error_msg("number too big"); continue;}
<INITIAL>{number}          {adjust(); yylval.val.number = atoi(yytext); return numbersym;}
<INITIAL>{ident_too_long}  {adjust(); show_error_msg("identifier is too long"); continue;}
<INITIAL>{ident}           {adjust(); yylval.val.ident = String(yytext); return identsym;}
<INITIAL>.                 {adjust(); show_error_msg("illegal token"); continue;}