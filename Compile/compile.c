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
//|  File:		driver.c
//|  Purpose:	attends to the overall flow of the lexer, parser,
//|             code gen, and running of code on the vm.
//|
//|  Notes:
//|===========================================================================

#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "parser.h"
#include "error.h"
#include "lexer.h"
#include "vm.h"

int main(int argc, char* argv[]) {

    // Declare and Initialize Variables:
    aToken_type tok;

    // Open and Declare File Pointers:
    FILE *source;
    FILE *clean;

    if (argc != 3){
        /*print cmd line args to std out
        printf("argc = %d\n", argc);
        printf("argv[0] = %s\n", argv[0]);
        printf("argv[1] = %s\n", argv[1]);
        printf("argv[2] = %s\n", argv[2]);
        */
        printf("invalid format, please use: ./compile <input> <output>\n");
        return 0;
    }

    // remove the following line for command line execution
    argv[1] = "correct4.pl0";

    //open source file and writable clean file
    source = fopen(argv[1], "r");
    clean = fopen("clean.pl0", "w");
    if (source == NULL){
        printf("Unable to open source file at %s/%s.\n",argv[0], argv[1]);
        return 0;
    }
    if (clean == NULL){
        printf("Unable to open clean file for writing.\n");
        return 0;
    }

    //Remove Comments from Source File, print to clean:
    removeComments(source, clean);

    // close source file & writable Clean file
    fclose(source);
    fclose(clean);

    // Have clean point to the new, clean file read-only:
    clean = fopen("loop_mult.pl0", "r");

    // run the lexer
    do_lex(clean);

    /*print tokens to std out
    while(true)
    {
        // Get the tokens from the shared array of tokens generated by lexer
        tok = nextToken();
        // halt if nullsym is returned
        if(tok.t == 1)
            break;
        // Displays the appropriate token
        displayToken(tok);
    }*/

    //printf("\nParser output:\n------\n");

    // Back to the beginning
    tok = rewindParseTokens();

    // runs the parser & code generator
    program(tok);

    //print the symbol table to std out
    printf("\n-------------------SYMBOLS-------------------\n");
    print_symboltable();

    // Close the cleanFile pointer
    fclose(clean);

    //Open write file for PM0 code
    FILE* codeFile = fopen(argv[2], "w+");
    if (codeFile == NULL){
        printf("Could not open file %s for writing\n", argv[2]);
        return 0;
    }

    //print to stdout and codeFile
    //printf("\n\nPM0 output:\n------\n");
    print_pm0(codeFile);

    printf("Compiled PM0 code written to %s\n", argv[2]);

    return 0;
}




