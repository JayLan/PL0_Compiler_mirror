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
//|  File:		error.h
//|  Purpose:	Provides enum typedef for errors and a function for printing
//|             out their error messages.
//|
//|  Notes:
//|
//|===========================================================================

#include "errorsss.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>

void print(char* message){
    printf(message);
}

bool error(int errnum){
    switch (errnum){
        //Error 1
        case becomesErr:
            print("Use = instead of :=\n");
            break;
        //Error 2
        case expNumErr:
            print("= must be followed by a number.\n");
            break;
        //Error 3
        case ExpEqlErr:
            print("Identifier must be followed by =\n");
            break;
        //Error 4
        case ExpIdentErr:
            print("'const', 'var', 'procedure' must be followed by identifier\n");
            break;
        //Error 5
        case expCommaSemicolErr:
            print("Semicolon or comma missing.\n");
            break;
        //Error 6
        case procDeclErr:
            print("Incorrect symbol after procedure declaration.\n");
            break;
        //Error 7
        case expStmtErr:
            print("Statement expected.\n");
            break;
        //Error 8
        case badSymAfterStmtBlkErr:
            print("Incorrect symbol after statement part in block.\n");
            break;
        //Error 9
        case expPeriodErr:
            print("Period expected.\n");
            break;
        //Error 10
        case missingSemicolErr:
            print("Semicolon between statements missing.\n");
            break;
        //Error 11
        case undeclIdentErr:
            print("Undeclared identifier.\n");
            break;
        //Error 12
        case improperAssnErr:
            print("Assignment to constant or procedure is not allowed.\n");
            break;
        //Error 13
        case expAssnOpErr:
            print("Assignment operator expected.\n");
            break;
        //Error 14
        case identAfterCallErr:
            print("'call' must be followed by an identifier.\n");
            break;
        //Error 15
        case callErr:
            print("Call of a constant or variable is meaningless.\n");
            break;
        //Error 16
        case expThenErr:
            print("'then' expected.\n");
            break;
        //Error 17
        case expEndOrSemicolErr:
            print("Semicolon or 'end' expected\n");
            break;
        //Error 18
        case expDoErr:
            print("'do' expected.\n");
            break;
        //Error 19
        case badSymAfterStmtErr:
            print("Incorrect symbol following statement.\n");
            break;
        //Error 20
        case expRelOpErr:
            print("Relational operator expected.\n");
            break;
        //Error 21
        case procIdentInExpErr:
            print("Expression must not contain a procedure identifier\n");
            break;
        //Error 22
        case expRightParenErr:
            print("Right parenthesis missing.\n");
            break;
        //Error 23
        case badFactorSymErr:
            print("The preceding factor cannot begin with this symbol.\n");
            break;
        //Error 24
        case badExpSymErr:
            print("An expression cannot begin with this symbol.\n");
            break;
        //Error 25
        case numTooLargeErr:
            print("This number is too large.\n");
            break;
    }
    return true;
}
