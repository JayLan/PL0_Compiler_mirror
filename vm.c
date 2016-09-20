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
};

/*function prototypes*/
int readFile_e(char* filename, struct instruction* program);
void translate_Assembly (int opcode, int lexi, int modifier);
void initialize_registers (struct registers* ptr);
void fetch(struct instruction instr_array [], struct registers* reg, int* stack);
int execute(struct registers* reg, int* stack);
int base (int level, int b, int* reg);
void print_Stack(struct registers* reg, int* stack);

int main(int argc, char** argv)
{
    //Declare and Initialize Variables:
    struct instruction instr_array [MAX_CODE_LENGTH];
    struct registers* reg;
    initialize_registers(reg);

    int* stack = (int*)calloc(MAX_STACK_HEIGHT, sizeof(int));


    //Begin Print Statement for PL/0 Code:
    printf("PL/0 Code: \n\n");

    //Scan in input from file and store in instruction array.
    //Send scanned in data to translation function.
    readFile_e(argv[1], instr_array);

    //Begin printing and formatting for the execution output:
    printf("\n\nExecution: \n\n");
    printf("\t \t \t \t pc\t bp\t sp\t stack\n");
    printf("\t \t \t \t %d\t %d\t %d\t \n", reg->pc, reg->bp, reg->sp);

    //Begin Fetch and Execution Cycles:
    fetch(instr_array, reg, stack);

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

void initialize_registers (struct registers* ptr){
    ptr = (struct registers*)calloc(1, sizeof(struct registers));
    ptr->sp = 0;
    ptr->bp = 1;
    ptr->pc = 0;
    //ptr->ir =
}

//=========================================================================
//=========================================================================

//Fetch Cycle:
//Pre-Conditions: Takes in an array of instructions, the stack pointer, and the number of lines.
//Post-Conditions: Fetches the instruction and stores it in the instruction register,
//                 calls execution function, and increments the program counter

void fetch(struct instruction instr_array [], struct registers* reg, int* stack){

    //Declare and Initialize Variables:
    int i = 0;
    int halt = 0;
    while (!halt){

        //If-Else statement ensures proper formatting in output.
        if (reg->pc < 10)
            printf(" %d\t", reg->pc);
        else
            printf("%d\t", reg->pc);


        //FETCH CYCLE:
        //Fetch the proper instruction register
        reg->ir = instr_array[reg->pc];
        //increment PC
        reg->pc++;

        //Check that PC is inbounds
        if (reg->pc > MAX_CODE_LENGTH)
            halt = 1;

        else{
            //Print out operation, lexicographical level, and modifier:
            translate_Assembly((reg->ir).op, (reg->ir).l, (reg->ir).m);

            //EXECUTION CYCLE:
            //Use the opcode to determine which operation to perform, then execute accordingly:
            halt = execute(reg, stack);

            //Display program counter, base pointer, stack pointer and stack in appropriate format:
            printf("\t %d\t %d\t %d\t", reg->pc, reg->bp, reg->sp);
            print_Stack(reg, stack);
        }
    }
}

/* read pl0 code from text input file                   */
/* returns 0 on success, 1 on failure (file not opened) */
int readFile_e(char* filename, struct instruction* instr_array)
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
        instr_array[numInstructions].op = n0;
        instr_array[numInstructions].l  = n1;
        instr_array[numInstructions].m  = n2;
        numInstructions++;
    }

    /* mark end of program code */
    numInstructions--;
    instr_array[numInstructions].op = 0;
    instr_array[numInstructions].l  = 0;
    instr_array[numInstructions].m  = 0;

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

int execute(struct registers* reg, int* stack){ //***************************

    switch((reg->ir).op) {

            //LIT: Pushes value M onto the stack:
            case 1:
                reg->sp = reg->sp + 1;
                stack[reg->sp] = (reg->ir).m; //***************** change reg->items to stack
                break;

            //OPR: Performs an arithmetic or logical operation as defined by
            //     the instruction registers modifier.
            case 2:

                //RET
                if ((reg->ir).m == 0){
                    reg->sp = (reg->bp) - 1;
                    reg->pc = stack[(reg->sp) + 4];
                    reg->bp = stack[(reg->sp) + 3];
                    break;
                }

                //NEG
                if ((reg->ir).m == 1){
                    stack[reg->sp] = -(stack[reg->sp]);
                    break;
                }

                //ADD
                if ((reg->ir).m == 2){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] + stack[(reg->sp) + 1];
                    break;
                }

                //SUB
                if ((reg->ir).m == 3){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] - stack[(reg->sp) + 1];
                    break;
                }

                //MUL
                if ((reg->ir).m == 4){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] * stack[(reg->sp) + 1];
                    break;
                }

                //DIV
                if ((reg->ir).m == 5){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] / stack[(reg->sp) + 1];
                    break;
                }

                //ODD
                if ((reg->ir).m == 6){
                    stack[reg->sp] = stack[reg->sp] % 2;
                    break;
                }

                //MOD
                if ((reg->ir).m == 7){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] % stack[(reg->sp) + 1];
                    break;
                }

                //EQL
                if ((reg->ir).m == 8){
                    reg->sp = (reg->sp) - 1;
                    stack[(reg->sp)] = stack[reg->sp] == stack[(reg->sp) + 1];
                    break;
                }

                //NEQ
                if ((reg->ir).m == 9){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] != stack[(reg->sp) + 1];
                    break;
                }

                //LSS
                if ((reg->ir).m == 10){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] < stack[(reg->sp) + 1];
                    break;
                }

                //LEQ
                if ((reg->ir).m == 11){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] <= stack[(reg->sp) + 1];
                    break;
                }

                //GTR
                if ((reg->ir).m == 12){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] > stack[(reg->sp) + 1];
                    break;
                }

                //GEQ
                if ((reg->ir).m == 13){
                    reg->sp = (reg->sp) - 1;
                    stack[reg->sp] = stack[reg->sp] >= stack[(reg->sp) + 1];
                    break;
                }

            //LOD: Get value at offset M in frame L levels down and push it.
            case 3:
                reg->sp = (reg->sp) + 1;
                stack[reg->sp] = stack[base((reg->ir).l, reg->bp, stack) + (reg->ir).m];
                break;

            //STO: Pop  and insert value at offset M in frame L levels down.
            case 4:
                stack[base((reg->ir).l, reg->bp, stack) + (reg->ir).m] = stack[reg->sp];
                reg->sp = (reg->sp) - 1;
                break;

            //CAL: Call procedure at M (generates new stack frame)
            case 5:
                stack[(reg->sp)+1] = 0;
                stack[(reg->sp)+2] = base((reg->ir).l, reg->bp, stack);
                stack[(reg->sp)+3] = reg->bp;
                stack[(reg->sp)+4] = reg->pc;
                reg->bp = (reg->sp) + 1;
                reg->pc = (reg->ir).m;
                break;

            //INC: Allocate M locals on stack
            case 6:
                reg->sp = (reg->sp) + (reg->ir).m;
                break;

            //JMP: Jump to M
            case 7:
                reg->pc = (reg->ir).m;
                break;

            //JPC: Pop stack and jump to M if value is equal to 0
            case 8:
                if (stack[reg->sp] == 0)
                    reg->pc = (reg->ir).m;
                reg->sp = (reg->sp) - 1;
                break;

            //SIO:
            case 9:

                //OUT: Pop stack and print out value
                if ((reg->ir).m == 0){
                    printf("%d", stack[reg->sp]);
                    reg->sp = (reg->sp) - 1;
                }

                //INP: Read input from user and push it
                if ((reg->ir).m == 1){
                    reg->sp = (reg->sp) + 1;
                    scanf("%d", &(stack[reg->sp]));
                }

                //HLT: Halt the machine
                if ((reg->ir).m == 2){
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

int base(int level, int b, int* stack){
    while (level > 0){
        b = stack[b + 2];
        level--;
    }

    return b;

}

//=========================================================================
//=========================================================================

//Print Stack:
//Pre-Conditions: Takes in a valid stack pointer.
//Post-Conditions: Prints the contents of the stack to the screen.

void print_Stack(struct registers* reg, int* stack){

    //Declare and Initialize Variables:
    int i;

    for (i = 1; i <= reg->sp; i++){

        if (i == reg->bp && i > 3)
            printf("|");

        printf("%d ", stack[i]);

    }

    printf("\n");

}

//=========================================================================
//=========================================================================

