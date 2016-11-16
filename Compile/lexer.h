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

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

int do_lex(FILE* clean);
aToken_type* getNextToken(FILE* cleanFile);
int  removeComments (FILE* infile, FILE* cleanFile);
void displayError(int code, int var);
void displaySourceFile(FILE* ifp);
void freeToken(aToken_type* t);

#endif // LEXER_H
