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
//|  Purpose:	Provides enum for token types and an array to store output
//|             tokens from the lexer, to be read by the parser
//|  Notes:
//|
//|===========================================================================

#ifndef TOKENS_H
#define TOKENS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TOKEN_ARRAY_SIZE 500
#define MAX_IDENT_LENGTH 12

typedef enum token {
  invalidsym = 0,
  nulsym,
  identsym,     //2
  numbersym,    //3
  plussym,      //4
  minussym,     //5
  multsym,      //6
  slashsym,     //7
  oddsym,       //8
  eqsym,        //9
  neqsym,       //10
  lessym,       //11
  leqsym,       //12
  gtrsym,       //13
  geqsym,       //14
  lparentsym,   //15
  rparentsym,   //16
  commasym,     //17
  semicolonsym, //18
  periodsym,    //19
  becomessym,   //20
  beginsym,     //21
  endsym,       //22
  ifsym,        //23
  thensym,      //24
  whilesym,     //25
  dosym,        //26
  callsym,      //27
  constsym,     //28
  varsym,       //29
  procsym,      //30
  writesym,     //31
  readsym,      //32
  elsesym       //33
} token_type;

//token value may be
union token_value
{
    int   number;
    char* identifier;
};

//token data type
typedef struct aToken_type
{
    union token_value val;
    token_type  t;

} aToken_type;

bool addToken(aToken_type* t);
aToken_type nextToken();
aToken_type rewindParseTokens();
void  displayToken(aToken_type t);

aToken_type* tokArr;
int lexCtr;
int parseCtr;

#endif // TOKENS_H
