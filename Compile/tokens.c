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

#ifndef TOKEN_GLOBALS
#define TOKEN_GLOBALS
aToken_type* tokArr;
int lexCtr;
int parseCtr = -1;
#endif // TOKEN_GLOBALS

bool addToken(aToken_type* t){

    //initialize tokCtr if necessary
    if(tokArr == NULL){
        tokArr = (aToken_type*) malloc(sizeof(aToken_type)*TOKEN_ARRAY_SIZE);
        lexCtr = 0;
    }

    //add the token to the array
    token_type n = t->t;
    tokArr[lexCtr].t = n;

    //add string ident or value if needed
    if(n ==2 ){
        tokArr[lexCtr].val.identifier = (char*) malloc(sizeof(char) * MAX_IDENT_LENGTH);
        strcpy( tokArr[lexCtr].val.identifier, t->val.identifier );
    }
    else if(n == 3){
        tokArr[lexCtr].val.number = t->val.number;
    }

    lexCtr++;

    return true;
}

aToken_type nextToken(){
    //initialize tokCtr if necessary
    if(parseCtr == -1){
        //printf("initializing parseCtr = 0\n");
        parseCtr = 0;
    }

    //return token and increment
    return tokArr[parseCtr++];
}

aToken_type rewindParseTokens(){
    parseCtr = 0;

    return tokArr[parseCtr];
}

void  displayToken(aToken_type t){

    //Declare an array with possible token types.
    /*const char* tokenTypeNames[] = {
        "PLACE_HOLDER",//  to align other values with enum token_type (starts at 1)
        "nulsym",   //  1
        "identsym", //  2
        "numbersym",//  3
        "+",        //  4
        "-",        //  5
        "*",        //  6
        "/",        //  7
        "odd",      //  8
        "=",        //  9
        "<>",       // 10
        "<",        // 11
        "<=",       // 12
        ">",        // 13
        ">=",       // 14
        "(",        // 15
        ")",        // 16
        ",",        // 17
        ";",        // 18
        ".",        // 19
        ":=",       // 20
        "begin",    // 21
        "end",      // 22
        "if",       // 23
        "then",     // 24
        "while",    // 25
        "do",       // 26
        "call",     // 27
        "const",    // 28
        "var",      // 29
        "procedure",// 30
        "write",    // 31
        "read",     // 32
        "else" };   // 33 */

    printf("%d ", t.t);

    //display token string or int value
    if(t.t == 2){
        printf("%s ", t.val.identifier);
    }else if(t.t == 3){
        printf("%d ", t.val.number);
    }


    return;

} // END display token

