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
typedef struct aToken_type
{
    union token_value val;
    token_type  t;

} aToken_type;

//array to store tokens output by lexer
aToken_type* tokArr;
static int lexCtr;
static int parseCtr;

bool addToken(aToken_type* t){
    //initialize tokCtr if necessary
    #ifndef tokArr
        tokArr = malloc(sizeof(aToken_type)*TOKEN_ARRAY_SIZE);
        lexCtr = 0;
    #endif // lexCtr

    //add the token to the array
    memcpy(t, &tokArr[lexCtr], sizeof(aToken_type));
    lexCtr++;

    return true;
}

aToken_type* nextToken(){
    //initialize tokCtr if necessary
    #ifndef parseCtr
        parseCtr = 0;
    #endif // parseCtr

    //return token and increment
    return &tokArr[parseCtr++];
}

aToken_type* rewindParseTokens(){
    parseCtr = 0;
    return &tokArr[parseCtr];
}

aToken_type* rewindLexTokens(){
    lexCtr = 0;
    return &tokArr[lexCtr];
}


#endif // TOKENS_H
