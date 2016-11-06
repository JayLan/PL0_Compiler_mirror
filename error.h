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

#ifndef ERROR_H
#define ERROR_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
  becomesErr = 1, expNumErr, ExpEqlErr, ExpIdentErr, expCommaSemicolErr,
  procDeclErr, expStmtErr, badSymAfterStmtBlkErr, expPeriodErr, missingSemicolErr,
  undeclIdentErr, improperAssnErr, expAssnOpErr, identAfterCallErr, callErr,
  expThenErr, expEndOrSemicolErr, expDoErr, badSymAfterStmtErr, expRelOpErr,
  procIdentInExpErr, expRightParenErr, badFactorSymErr, badExpSymErr, numTooLargeErr
} err_type;

void print(char* message){
    printf(message);
}

bool error(int errnum){
    switch (errnum){
        //Error 1
        case becomesErr:
            print("Use = instead of :=");
            break;
        //Error 2
        case expNumErr:
            print("= must be followed by a number.");
            break;
        //Error 3
        case ExpEqlErr:
            print("Identifier must be followed by =");
            break;
        //Error 4
        case ExpIdentErr:
            print("'const', 'var', 'procedure' must be followed by identifier");
            break;
        //Error 5
        case expCommaSemicolErr:
            print("Semicolon or comma missing.");
            break;
        //Error 6
        case procDeclErr:
            print("Incorrect symbol after procedure declaration.");
            break;
        //Error 7
        case expStmtErr:
            print("Statement expected.");
            break;
        //Error 8
        case badSymAfterStmtBlkErr:
            print("Incorrect symbol after statement part in block.");
            break;
        //Error 9
        case expPeriodErr:
            print("Period expected.");
            break;
        //Error 10
        case missingSemicolErr:
            print("Semicolon between statements missing.");
            break;
        //Error 11
        case undeclIdentErr:
            print("Undeclared identifier.");
            break;
        //Error 12
        case improperAssnErr:
            print("Assignment to constant or procedure is not allowed.");
            break;
        //Error 13
        case expAssnOpErr:
            print("Assignment operator expected.");
            break;
        //Error 14
        case identAfterCallErr:
            print("'call' must be followed by an identifier.");
            break;
        //Error 15
        case callErr:
            print("Call of a constant or variable is meaningless.");
            break;
        //Error 16
        case expThenErr:
            print("'then' expected.");
            break;
        //Error 17
        case expEndOrSemicolErr:
            print("Semicolon or 'end' expected");
            break;
        //Error 18
        case expDoErr:
            print("'do' expected.");
            break;
        //Error 19
        case badSymAfterStmtErr:
            print("Incorrect symbol following statement.");
            break;
        //Error 20
        case expRelOpErr:
            print("Relational operator expected.");
            break;
        //Error 21
        case procIdentInExpErr:
            print("Expression must not contain a procedure identifier");
            break;
        //Error 22
        case expRightParenErr:
            print("Right parenthesis missing.");
            break;
        //Error 23
        case badFactorSymErr:
            print("The preceding factor cannot begin with this symbol.");
            break;
        //Error 24
        case badExpSymErr:
            print("An expression cannot begin with this symbol.");
            break;
        //Error 25
        case numTooLargeErr:
            print("This number is too large.");
            break;
    }
    return true;
}

#endif
