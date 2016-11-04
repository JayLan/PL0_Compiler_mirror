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
//|  File:		parser.c
//|  Purpose:	Assignment #3, Implement a parser
//|
//|  Notes:
//|
//|===========================================================================

//Included Libraries
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "lexer.h"

int main () {

    //Declare and Initialize Variables:
    aToken_type *toke;

    //Open and Declare File Pointers:
    FILE *source;
    FILE *clean;

    source = fopen("tester.txt", "r");
    clean = fopen("clean.pl0", "w");

    //Check if pointers are valid:
    if (source == NULL){
        printf("Unable to open file.\n");
    }

    if (clean == NULL){
        printf("Unable to open file.\n");
    }

    //Remove Comments from Source File:
    removeComments(source, clean);

    fclose(source);
    fclose(clean);

    //Have Clean point to the new, clean file:
    clean = fopen("clean.pl0", "r");

    while(true)
    {
        //Get the tokens from the clean file.
        toke = getNextToken(clean);

        // halt if nullsym is returned
        if ( toke->t == 1 )
            break;

        //Display the appropriate token, then free it
        displayToken(toke);
        freeToken(toke);
    }

    //Close the cleanFile pointer
    fclose(clean);
}


