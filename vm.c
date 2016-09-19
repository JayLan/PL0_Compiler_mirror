//|===========================================================================
//|  Team Name: Compiler Builders 33
//|
//|  Programmer(s):
//|		Cristen Palmer
//|		Megan Chipman
//|		Jason Lancaster
//|		Victoria Proetsch
//|
//|  Course:	COP3402, Fall 2016
//|  Date:
//|---------------------------------------------------------------------------
//|  Language:	C
//|  File:		vm.c
//|  Purpose:	Assignment #1, Implement a virtual machine for Pl/0
//|
//|  Notes:
//|
//|===========================================================================

#include <stdio.h>
#include <stdlib.h>

/* constants */
const int MAX_STACK_HEIGHT = 2000;
const int MAX_CODE_LENGTH  =  500;
const int MAX_LEXI_LEVELS  =    3;

/* structures */
struct instruction {
    int op;
    int  l;
    int  m;
};

struct registers {
    int sp;
    int bp;
    int pc;
    struct instruction ir;
}

/*function prototypes*/
int readFile_e(char* filename, struct instruction* program);

int main(int argc, char**argv)
{
    //Declare and Initialize Variables:
    FILE *ifp = fopen(argv[1], "r");
    struct instruction instArray [MAX_CODE_LENGTH];
    stack ptr;
    int line = 0;

    initialize_Stack(&ptr);

    //Begin Print Statement for PL/0 Code:
    printf("PL/0 Code: \n\n");

    //Scan in input from file and store in instruction array.
    //Send scanned in data to translation function.
    readFile_e(ifp, instArray);

    //Begin printing and formatting for the execution output:
    printf("\n\nExecution: \n\n");
    printf("\t \t \t \t pc\t bp\t sp\t stack\n");
    printf("\t \t \t \t %d\t %d\t %d\t \n", ptr.pc, ptr.bp, ptr.sp);

    //Begin Fetch and Execution Cycles:
    fetch(instArray, &ptr, line);

    //Close File Pointer
    fclose(ifp);

    return 0;
}

/* read pl0 code from text input file                   */
/* returns 0 on success, 1 on failure (file not opened) */
int readFile_e(char* filename, struct instruction* instArray)
{
    int numInstructions = 0;
    int n0 = 0, n1 = 0, n2 = 0;
    FILE *fp;
    char *mode = "r";

    fp = fopen(filename, mode);

    if (fp == NULL)
    {
        return 1;
    }

    /* read until eof or max instructions read */
    while ( (numInstructions < MAX_CODE_LENGTH) && !feof(fp) )
    {
        fscanf(fp, "%d %d %d", &n0, &n1, &n2);
        instArray[numInstructions].op = n0;
        instArray[numInstructions].l  = n1;
        instArray[numInstructions].m  = n2;
        numInstructions++;
    }

    /* mark end of program code */
    numInstructions--;
    program[numInstructions].op = 0;
    program[numInstructions].l  = 0;
    program[numInstructions].m  = 0;

    /* close file */
    fclose(fp);

    return 0;
}
