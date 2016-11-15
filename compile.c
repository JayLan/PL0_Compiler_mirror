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
        printf("argc = %d\n", argc);
        printf("argv[0] = %s\n", argv[0]);
        printf("argv[1] = %s\n", argv[1]);
        printf("argv[2] = %s\n", argv[2]);
        printf("invalid format, please use: ./compile <input> <output>\n");
        return;
    }

    // remove the following line for command line execution
    argv[1] = "tester2.pl0";

    source = fopen(argv[1], "r"); //replace filename with argv[1] for command line testing
    clean = fopen("clean.pl0", "w");

    // Check if pointers are valid:
    if (source == NULL){
        printf("Unable to open source file at %s/%s.\n",argv[0], argv[1]);
        return;
    }

    if (clean == NULL){
        printf("Unable to open clean file for writing.\n");
        return;
    }

    //Remove Comments from Source File:
    removeComments(source, clean);

    // close source file & writable Clean file
    fclose(source);
    fclose(clean);

    // Have clean point to the new, clean file read-only:
    clean = fopen("clean.pl0", "r");

    // run the lexer
    do_lex(clean);

    while(true)
    {
        // Get the tokens from the shared array of tokens generated by lexer
        tok = nextToken();

        // halt if nullsym is returned
        if(tok.t == 1)
            break;

        // Displays the appropriate token
        displayToken(tok);

    }

    printf("\n\nParser output:\n------\n");

    // Back to the beginning
    tok = rewindParseTokens();

    // runs the parser & code generator
    program(tok);

    //prints the symbol table
    printf("\n-------------------SYMBOLS-------------------\n");
    print_symboltable();

    // Close the cleanFile pointer
    fclose(clean);

    //Open write file for PM0 code
    FILE* codeFile = fopen(argv[2], "w+");

    //print to stdout and codeFile
    printf("\n\nPM0 output:\n------\n");
    print_pm0(codeFile);

}





