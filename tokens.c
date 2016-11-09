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
