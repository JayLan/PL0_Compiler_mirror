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
//|  File:		tokens.h
//|  Purpose:	Provides enum for token types and an array to store output
//|             tokens from the lexer, to be read by the parser
//|  Notes:
//|
//|===========================================================================

#ifndef VMSSS_H
#define VMSSS_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_CODE_LENGTH 500

typedef enum {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
} opType;

typedef enum {
    RET = 0, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL,
    NEQ, LSS, LEQ, GTR, GEQ
} opMod;

typedef enum {
    OUT = 0, INP, HLT
} sioType;

typedef struct instruction
{
    int op;
    int  l;
    int  m;
} instruction;

#endif
