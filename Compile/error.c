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


void error(int errnum){
    printf("\nPARSE ERROR %d: ", errnum);
    switch (errnum){
        case 1:
            printf("Use = instead of :=");
            break;
        case 2:
            printf("= must be followed by a number.");
            break;
        case 3:
            printf("Identifier must be followed by =");
            break;
        case 4:
            printf("'const', 'var', 'procedure' must be followed by identifier");
            break;
        case 5:
            printf("Semicolon or comma missing.");
            break;
        case 6:
            printf("Incorrect symbol after procedure declaration.");
            break;
        case 7:
            printf("Statement expected.");
            break;
        case 8:
            printf("Incorrect symbol after statement part in block.");
            break;
        case 9:
            printf("Period expected.");
            break;
        case 10:
            printf("Semicolon between statements missing.");
            break;
        case 11:
            printf("Undeclared identifier.");
            break;
        case 12:
            printf("Assignment to constant or procedure is not allowed.");
            break;
        case 13:
            printf("Assignment operator expected.");
            break;
        case 14:
            printf("'call' must be followed by an identifier.");
            break;
        case 15:
            printf("Call of a constant or variable is meaningless.");
            break;
        case 16:
            printf("'then' expected.");
            break;
        case 17:
            printf("'end' expected");
            break;
        case 18:
            printf("'do' expected.");
            break;
        case 19:
            printf("Incorrect symbol following statement.");
            break;
        case 20:
            printf("Relational operator expected.");
            break;
        case 21:
            printf("Expression must not contain a procedure identifier");
            break;
        case 22:
            printf("Right parenthesis missing.");
            break;
        case 23:
            printf("The preceding factor cannot begin with this symbol.");
            break;
        case 24:
            printf("An expression cannot begin with this symbol.");
            break;
        case 25:
            printf("This number is too large.");
            break;
        case 26:
            printf("The code is too long.");
            break;
        case 27:
            printf("'read' must be followed by an identifier.");
            break;
        case 28:
            printf("'write' must be followed by a variable or constant identifier.");
            break;
        default:
            printf("wooooo mystery error (try a number 1-28)");
            break;
    }
    exit(1);
    return;
}
