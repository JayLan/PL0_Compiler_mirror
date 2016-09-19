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
void translate_Assembly (int opcode, int lexi, int modifier);
void initialize_Stack (stack *ptr);
void fetch (struct instruction instArray [], stack *stackPtr, int line);
int execute (stack **stackPtr);
int base (int level, int b, stack **stackPtr);
void print_Stack (stack **stackPtr);

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

//=========================================================================

//Translate Assembly
//Pre-Conditions: Takes in integer variables representing the opcode, lexicographical level,
//                and modifier values for a line of PL/0 code.
//Post-Conditions: Displays the assembly version of the PL/0 line of code.

void translate_Assembly(int opcode, int lexi, int modifier){

    //Implement switch statements based on opcode for correct print out.
    switch (opcode) {

        //LIT
        case 1:
                printf("LIT\t  \t %d", modifier);
                break;

        //OPR
        case 2:
                if(modifier == 0)
                    printf("RET\t  \t %d", modifier);

                if(modifier == 1)
                    printf("NEG\t  \t %d", modifier);

                if(modifier == 2)
                    printf("ADD\t  \t %d", modifier);

                if(modifier == 3)
                    printf("SUB\t  \t %d", modifier);

                if(modifier == 4)
                    printf("MUL\t  \t %d", modifier);

                if(modifier == 5)
                    printf("DIV\t  \t %d", modifier);

                if(modifier == 6)
                    printf("ODD\t  \t %d", modifier);

                if(modifier == 7)
                    printf("MOD\t  \t %d", modifier);

                if(modifier == 8)
                    printf("EQL\t  \t %d", modifier);

                if(modifier == 9)
                    printf("NEQ\t  \t %d", modifier);

                if(modifier == 10)
                    printf("LSS\t  \t %d", modifier);

                if(modifier == 11)
                    printf("LEQ\t  \t %d", modifier);

                if(modifier == 12)
                    printf("GTR\t  \t %d", modifier);

                if(modifier == 13)
                    printf("GEQ\t  \t %d", modifier);

                break;

        //LOD
        case 3:
                printf("LOD\t %d\t %d", lexi, modifier);
                break;

        //STO
        case 4:
                printf("STO\t %d\t %d", lexi, modifier);
                break;

        //CAL
        case 5:
                printf("CAL\t %d\t %d", lexi, modifier);
                break;

        //INC
        case 6:
                printf("INC\t  \t %d", modifier);
                break;

        //JMP
        case 7:
                printf("JMP\t  \t %d", modifier);
                break;

        //JPC
        case 8:
                printf("JPC\t  \t %d", modifier);
                break;

        //SIO
        case 9:
                if (modifier == 0)
                    printf("OUT\t \t %d", modifier);
                if (modifier == 1)
                    printf("INP\t \t %d", modifier);
                if (modifier == 2)
                    printf("HLT");
                break;
    }

}

//=========================================================================
//=========================================================================

//Pre-Conditions: Takes in a valid stack pointer.
//Post-Conditions: Initializes the stack using numbers described in project description.

void initialize_Stack (struct stack *ptr){

    ptr->sp = 0;
    ptr->bp = 1;
    ptr->pc = 0;
}

//=========================================================================
//=========================================================================

//Fetch Cycle:
//Pre-Conditions: Takes in an array of instructions, the stack pointer, and the number of lines.
//Post-Conditions: Fetches the instruction and stores it in the instruction register,
//                 calls execution function, and increments the program counter

void fetch(struct instruction instArray [], struct stack *stackPtr, int line){

    //Declare and Initialize Variables:
    int i = 0;
    int halt = 0;
    while (!halt){

        //If-Else statement ensures proper formatting in output.
        if (stackPtr->pc < 10)
            printf(" %d\t", stackPtr->pc);
        else
            printf("%d\t", stackPtr->pc);


        //FETCH CYCLE:
        //Fetch the proper instruction register
        stackPtr->ir = instArray[stackPtr->pc];
        //increment PC
        stackPtr->pc++;

        //Check that PC is inbounds
        if (stackPtr->pc > MAX_CODE_LENGTH)
            halt = 1;

        else{
            //Print out operation, lexicographical level, and modifier:
            translate_Assembly((stackPtr->ir).op, (stackPtr->ir).l, (stackPtr->ir).m);

            //EXECUTION CYCLE:
            //Use the opcode to determine which operation to perform, then execute accordingly:
            halt = execute(&stackPtr);

            //Display program counter, base pointer, stack pointer and stack in appropriate format:
            printf("\t %d\t %d\t %d\t", stackPtr->pc, stackPtr->bp, stackPtr->sp);
            print_Stack(&stackPtr);
        }
    }
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

//=========================================================================

//Execution Cycle:
//Pre-Conditions: Takes in a valid stack pointer.
//Post-Conditions: Executes the appropriate instruction as identified by the
//                 instruction register.
//=========================================================================
//=========================================================================

int execute(struct stack **stackPtr){

    switch(((*stackPtr)->ir).op) {

            //LIT: Pushes value M onto the stack:
            case 1:
                (*stackPtr)->sp = (*stackPtr)->sp + 1;
                (*stackPtr)->items[(*stackPtr)->sp] = ((*stackPtr)->ir).m;
                break;

            //OPR: Performs an arithmetic or logical operation as defined by
            //     the instruction registers modifier.
            case 2:

                //RET
                if (((*stackPtr)->ir).m == 0){
                    (*stackPtr)->sp = ((*stackPtr)->bp) - 1;
                    (*stackPtr)->pc = (*stackPtr)->items[((*stackPtr)->sp) + 4];
                    (*stackPtr)->bp = (*stackPtr)->items[((*stackPtr)->sp) + 3];
                    break;
                }

                //NEG
                if (((*stackPtr)->ir).m == 1){
                    (*stackPtr)->items[(*stackPtr)->sp] = -((*stackPtr)->items[(*stackPtr)->sp]);
                    break;
                }

                //ADD
                if (((*stackPtr)->ir).m == 2){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] + (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //SUB
                if (((*stackPtr)->ir).m == 3){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] - (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //MUL
                if (((*stackPtr)->ir).m == 4){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] * (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //DIV
                if (((*stackPtr)->ir).m == 5){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] / (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //ODD
                if (((*stackPtr)->ir).m == 6){
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] % 2;
                    break;
                }

                //MOD
                if (((*stackPtr)->ir).m == 7){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] % (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //EQL
                if (((*stackPtr)->ir).m == 8){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[((*stackPtr)->sp)] = (*stackPtr)->items[(*stackPtr)->sp] == (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //NEQ
                if (((*stackPtr)->ir).m == 9){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] != (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //LSS
                if (((*stackPtr)->ir).m == 10){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] < (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //LEQ
                if (((*stackPtr)->ir).m == 11){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] <= (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //GTR
                if (((*stackPtr)->ir).m == 12){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] > (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

                //GEQ
                if (((*stackPtr)->ir).m == 13){
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                    (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[(*stackPtr)->sp] >= (*stackPtr)->items[((*stackPtr)->sp) + 1];
                    break;
                }

            //LOD: Get value at offset M in frame L levels down and push it.
            case 3:
                (*stackPtr)->sp = ((*stackPtr)->sp) + 1;
                (*stackPtr)->items[(*stackPtr)->sp] = (*stackPtr)->items[base(((*stackPtr)->ir).l, (*stackPtr)->bp, stackPtr) + ((*stackPtr)->ir).m];
                break;

            //STO: Pop stack and insert value at offset M in frame L levels down.
            case 4:
                (*stackPtr)->items[base(((*stackPtr)->ir).l, (*stackPtr)->bp, stackPtr) + ((*stackPtr)->ir).m] = (*stackPtr)->items[(*stackPtr)->sp];
                (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                break;

            //CAL: Call procedure at M (generates new stack frame)
            case 5:
                (*stackPtr)->items[((*stackPtr)->sp)+1] = 0;
                (*stackPtr)->items[((*stackPtr)->sp)+2] = base(((*stackPtr)->ir).l, (*stackPtr)->bp, stackPtr);
                (*stackPtr)->items[((*stackPtr)->sp)+3] = (*stackPtr)->bp;
                (*stackPtr)->items[((*stackPtr)->sp)+4] = (*stackPtr)->pc;
                (*stackPtr)->bp = ((*stackPtr)->sp) + 1;
                (*stackPtr)->pc = ((*stackPtr)->ir).m;
                break;

            //INC: Allocate M locals on stack
            case 6:
                (*stackPtr)->sp = ((*stackPtr)->sp) + ((*stackPtr)->ir).m;
                break;

            //JMP: Jump to M
            case 7:
                (*stackPtr)->pc = ((*stackPtr)->ir).m;
                break;

            //JPC: Pop stack and jump to M if value is equal to 0
            case 8:
                if ((*stackPtr)->items[(*stackPtr)->sp] == 0)
                    (*stackPtr)->pc = ((*stackPtr)->ir).m;
                (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                break;

            //SIO:
            case 9:

                //OUT: Pop stack and print out value
                if (((*stackPtr)->ir).m == 0){
                    printf("%d", (*stackPtr)->items[(*stackPtr)->sp]);
                    (*stackPtr)->sp = ((*stackPtr)->sp) - 1;
                }

                //INP: Read input from user and push it
                if (((*stackPtr)->ir).m == 1){
                    (*stackPtr)->sp = ((*stackPtr)->sp) + 1;
                    scanf("%d", &((*stackPtr)->items[(*stackPtr)->sp]));
                }

                //HLT: Halt the machine
                if (((*stackPtr)->ir).m == 2){
                    printf("\t\t");
                    return 1;
                }

    }
    return 0;

}

//=========================================================================
//=========================================================================

//Base:
//Pre-Conditions: Takes in a lexicographical level and an integer b.
//Post-Conditions: Computes and returns the AR base L levels down.

int base(int level, int b, struct stack **stackPtr){
    while (level > 0){
        b = (*stackPtr)->items[b + 2];
        level--;
    }

    return b;

}

//=========================================================================
//=========================================================================

//Print Stack:
//Pre-Conditions: Takes in a valid stack pointer.
//Post-Conditions: Prints the contents of the stack to the screen.

void print_Stack(struct stack **stackPtr){

    //Declare and Initialize Variables:
    int i;

    for (i = 1; i <= (*stackPtr)->sp; i++){

        if (i == (*stackPtr)->bp && i > 3)
            printf("|");

        printf("%d ", (*stackPtr)->items[i]);

    }

    printf("\n");

}

//=========================================================================
//=========================================================================

