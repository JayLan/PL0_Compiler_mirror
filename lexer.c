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

