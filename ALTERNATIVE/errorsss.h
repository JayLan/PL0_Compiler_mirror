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
//|  Purpose:	Provides enum typedef for errors and function prototypes for
//|             error.c
//|
//|  Notes:
//|
//|===========================================================================

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"

typedef enum {
  becomesErr = 1, expNumErr, ExpEqlErr, ExpIdentErr, expCommaSemicolErr,
  procDeclErr, expStmtErr, badSymAfterStmtBlkErr, expPeriodErr, missingSemicolErr,
  undeclIdentErr, improperAssnErr, expAssnOpErr, identAfterCallErr, callErr,
  expThenErr, expEndOrSemicolErr, expDoErr, badSymAfterStmtErr, expRelOpErr,
  procIdentInExpErr, expRightParenErr, badFactorSymErr, badExpSymErr, numTooLargeErr
} err_type;

void print(char* message);
bool error(int errnum);

#endif

