//|===========================================================================
//|  Team Name: Compiler Builders 33
//|
//|  Programmers:
//|		Megan Chipman
//|		Jason Lancaster
//|		Victoria Proetsch
//|
//|  Course:	COP3402, Fall 2016
//|  Date:      Sept 22, 2016
//|---------------------------------------------------------------------------
//|  Language:	C
//|  File:		tokens.h
//|  Purpose:	Provides enum for token types.
//|
//|  Notes:
//|
//|===========================================================================

#ifndef TOKENS_H
#define TOKENS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum token {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

//token value may be
union token_value
{
    int   number;
    char* identifier;
};

//token data type
typedef struct
{
    union token_value val;
    token_type  t;

} aToken_type;


//array to store tokens output by lexer
static aToken_type tok [255];

#endif // TOKENS_H
