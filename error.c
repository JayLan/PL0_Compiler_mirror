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
    printf("\nERROR %d: ", errnum);
    switch (errnum){
        case 1:
            print("Use = instead of :=");
            break;
        case 2:
            print("= must be followed by a number.");
            break;
        case 3:
            print("Identifier must be followed by =");
            break;
        case 4:
            print("'const', 'var', 'procedure' must be followed by identifier");
            break;
        case 5:
            print("Semicolon or comma missing.");
            break;
        case 6:
            print("Incorrect symbol after procedure declaration.");
            break;
        case 7:
            print("Statement expected.");
            break;
        case 8:
            print("Incorrect symbol after statement part in block.");
            break;
        case 9:
            print("Period expected.");
            break;
        case 10:
            print("Semicolon between statements missing.");
            break;
        case 11:
            print("Undeclared identifier.");
            break;
        case 12:
            print("Assignment to constant or procedure is not allowed.");
            break;
        case 13:
            print("Assignment operator expected.");
            break;
        case 14:
            print("'call' must be followed by an identifier.");
            break;
        case 15:
            print("Call of a constant or variable is meaningless.");
            break;
        case 16:
            print("'then' expected.");
            break;
        case 17:
            print("Semicolon or 'end' expected");
            break;
        case 18:
            print("'do' expected.");
            break;
        case 19:
            print("Incorrect symbol following statement.");
            break;
        case 20:
            print("Relational operator expected.");
            break;
        case 21:
            print("Expression must not contain a procedure identifier");
            break;
        case 22:
            print("Right parenthesis missing.");
            break;
        case 23:
            print("The preceding factor cannot begin with this symbol.");
            break;
        case 24:
            print("An expression cannot begin with this symbol.");
            break;
        case 25:
            print("This number is too large.");
            break;
        case 26:
            print("This number is too large.");
            break;
        default:
            print("wooooo mystery error (try a number 1-26)");
            break;
    }
    exit(1);
    return;
}
