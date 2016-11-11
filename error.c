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

#include "error.h"
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
            print("Use = instead of :=");
            exit(1);
            break;
        //Error 2
        case expNumErr:
            print("= must be followed by a number.");
            exit(1);
            break;
        //Error 3
        case ExpEqlErr:
            print("Identifier must be followed by =");
            exit(1);
            break;
        //Error 4
        case ExpIdentErr:
            print("'const', 'var', 'procedure' must be followed by identifier");
            exit(1);
            break;
        //Error 5
        case expCommaSemicolErr:
            print("Semicolon or comma missing.");
            exit(1);
            break;
        //Error 6
        case procDeclErr:
            print("Incorrect symbol after procedure declaration.");
            exit(1);
            break;
        //Error 7
        case expStmtErr:
            print("Statement expected.");
            exit(1);
            break;
        //Error 8
        case badSymAfterStmtBlkErr:
            print("Incorrect symbol after statement part in block.");
            exit(1);
            break;
        //Error 9
        case expPeriodErr:
            print("Period expected.");
            exit(1);
            break;
        //Error 10
        case missingSemicolErr:
            print("Semicolon between statements missing.");
            exit(1);
            break;
        //Error 11
        case undeclIdentErr:
            print("Undeclared identifier.");
            exit(1);
            break;
        //Error 12
        case improperAssnErr:
            print("Assignment to constant or procedure is not allowed.");
            exit(1);
            break;
        //Error 13
        case expAssnOpErr:
            print("Assignment operator expected.");
            exit(1);
            break;
        //Error 14
        case identAfterCallErr:
            print("'call' must be followed by an identifier.");
            exit(1);
            break;
        //Error 15
        case callErr:
            print("Call of a constant or variable is meaningless.");
            exit(1);
            break;
        //Error 16
        case expThenErr:
            print("'then' expected.");
            exit(1);
            break;
        //Error 17
        case expEndOrSemicolErr:
            print("Semicolon or 'end' expected");
            exit(1);
            break;
        //Error 18
        case expDoErr:
            print("'do' expected.");
            exit(1);
            break;
        //Error 19
        case badSymAfterStmtErr:
            print("Incorrect symbol following statement.");
            exit(1);
            break;
        //Error 20
        case expRelOpErr:
            print("Relational operator expected.");
            exit(1);
            break;
        //Error 21
        case procIdentInExpErr:
            print("Expression must not contain a procedure identifier");
            exit(1);
            break;
        //Error 22
        case expRightParenErr:
            print("Right parenthesis missing.");
            exit(1);
            break;
        //Error 23
        case badFactorSymErr:
            print("The preceding factor cannot begin with this symbol.");
            exit(1);
            break;
        //Error 24
        case badExpSymErr:
            print("An expression cannot begin with this symbol.");
            exit(1);
            break;
        //Error 25
        case numTooLargeErr:
            print("This number is too large.");
            exit(1);
            break;
    }
    return true;
}
