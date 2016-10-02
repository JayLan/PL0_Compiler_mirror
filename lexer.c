//|===========================================================================
//|  Team Name: Compiler Builders 33
//|
//|  Programmers:
//|		Cristen Palmer
//|		Megan Chipman
//|		Jason Lancaster
//|		Victoria Proetsch
//|
//|  Course:	COP3402, Fall 2016
//|  Date:      Oct 6, 2016
//|---------------------------------------------------------------------------
//|  Language:	C
//|  File:		lexer.c
//|  Purpose:	Assignment #2, Implement a lexical analyzer for PL/0
//|
//|  Notes:
//|
//| * Identifier cannot start with a letter
//| * Identifier cannot exceed 12 chars
//| * Number cannot exceed 2^16 - 1
//| * Token cannot be invalid
//| * When an above error is encountered, program must halt and state type of error
//| * Clean code removes comments but not superfluous whitespace/newlines
//|===========================================================================


#include <stdio.h>
#include <stdlib.h>
// * currently including tokens.h - may copy enum to code base to avoid makefile *
#include "tokens.h"

#define LEX_ERR_1 "*** Identifier Does Not Begin With a Letter ***"
#define LEX_ERR_2 "*** Identifier Exceeds 12 Characters ***"
#define LEX_ERR_3 "*** Number Exceeds Allowable Size ***"
#define LEX_ERR_4 "*** Invalid Token ***"



// Function Prototypes:
//void displaySource();
//void displaySourceClean();
//void displayInputFile(FILE* infile);

//* switch case this, do: puts(LEX_ERR_#) for each case if to console;
//void displayLexError(int err_code);


int main(int argc, char *argv[]) {









	return 0;

} // End Main 
