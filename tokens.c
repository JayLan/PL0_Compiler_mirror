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
//|  File:		tokens.c
//|  Purpose:	Provides enum typedef for errors and a function for printing
//|             out their error messages.
//|
//|  Notes:
//|
//|===========================================================================

#include "tokens.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

aToken_type* tokArr;
int lexCtr;
int parseCtr;

bool addToken(aToken_type* t){
    //initialize tokCtr if necessary
    if (tokArr == NULL){
        tokArr = malloc(sizeof(aToken_type)*TOKEN_ARRAY_SIZE);
        lexCtr = 0;
    }

    //printf("lexCtr = %d\n", lexCtr);

    //add the token to the array
    //memcpy(&tokArr[lexCtr], t, sizeof(aToken_type));
    token_type n = t->t;
    tokArr[lexCtr].t = n;
    if (n ==2 ){
        tokArr[lexCtr].val.identifier = malloc(sizeof(char) * IDENT_MAX_LENGTH);
        strcpy( tokArr[lexCtr].val.identifier, t->val.identifier );
    }
    else if (n == 3){
        tokArr[lexCtr].val.number = t->val.number;
    }
    //printf("copied t to array\n", lexCtr);

    lexCtr++;
    //printf("lexCtr after increment = %d\n", lexCtr);

    return true;
}

aToken_type* nextToken(){
    //initialize tokCtr if necessary
    if (parseCtr == NULL){
        parseCtr = 0;
    }

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
