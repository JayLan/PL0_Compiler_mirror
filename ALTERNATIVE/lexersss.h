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
//|  File:		lexer.h
//|  Purpose:	Provides function prototypes from lexer.c
//|
//|  Notes:
//|
//|===========================================================================

#include "tokensss.h"
#include <stdio.h>
#include <stdlib.h>

void displaySourceFile(FILE* ifp);
int  removeComments (FILE* infile, FILE* cleanFile);
void displayToken(aToken_type* t);
void freeToken(aToken_type* t);
aToken_type* getNextToken(FILE* cleanFile);
