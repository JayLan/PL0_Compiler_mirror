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
//|  Date:      Sept 22, 2016
//|---------------------------------------------------------------------------
//|  Language:	C
//|  File:		vm.c
//|  Purpose:	Assignment #1, Implement a virtual machine for PL/0
//|
//|  Notes:
//|
//|===========================================================================

#include <stdio.h>
#include <stdlib.h>

// constants
const int MAX_STACK_HEIGHT = 2000;
const int MAX_CODE_LENGTH  =  500;
const int MAX_LEXI_LEVELS  =    3;

const char opNames   [10][4] = {
    "XXX", "LIT", "OPR", "LOD", "STO", "CAL", "INC",
    "JMP", "JPC", "SIO" };

const char opNamesOPR[14][4] = {
    "RET", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD",
    "MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ" };

const char opNamesSIO [3][4] = {"OUT", "INP", "HLT" };

// structures
struct instruction
{
    int op;
    int  l;
    int  m;
};

struct registers
{
    int sp;
    int bp;
    int pc;
    struct instruction ir;
};

// function prototypes
int  ARheight(struct registers* reg, int* stack, int n);
int  base (int level, int b, int* reg);
int  readFile(char* filename, struct instruction* program);
int  execute(struct registers* reg, int* stack);
void fetch(struct instruction instr_array [], struct registers* reg, int* stack);
void initializeRegisters(struct registers* reg);
void print_Instruction(int op, int l, int m);
void print_Program(struct instruction* instr_array, char* filename);
void print_Stack(struct registers* reg, int* stack);


int main(int argc, char** argv)
{
    int maxWidth = 4;
    int* stack = (int*)calloc(MAX_STACK_HEIGHT, sizeof(int));
    char* filename = argv[1];
    struct instruction instr_array [MAX_CODE_LENGTH];
    struct registers* reg = (struct registers*)calloc(1, sizeof(struct registers));
    
    // scan in input from file and store in instruction array
    if( readFile(filename, instr_array) )
    {
        printf("couldn't open file %s.\n", argv[1]);
        
        return 1;
    }
    
    // print all program instructions
    print_Program(instr_array, filename);
    
    // begin printing and formatting for the execution output
    printf("Execution:\n");
    printf("%*s   pc   bp   sp   stack\n", 19, "");
    printf("%*s %*d %*d %*d\n", 19, "", maxWidth, reg->pc, maxWidth, reg->bp, maxWidth, reg->sp);
    
    // fetch and execute cycle
    fetch(instr_array, reg, stack);
    
    return 0;
}


// Pre-condition:  reg and stack are non-null pointers, n is valid ordinal
//                 number of an AR record in stack.
// Post-condition: Returns height of n^{th} AR record from bottom of stack.

int ARheight(struct registers* reg, int* stack, int n)
{
    int base   = reg->bp;
    int top    = reg->sp;
    int numARs = 1;
    
    // calculate number of AR records
    while(base > 1)
    {
        top = base - 1;
        base = stack[base+1];
        numARs++;
    }
    
    base = reg->bp;
    top  = reg->sp;
    
    int depth = numARs - n;
    
    // find top and bottom of n^{th} AR record from bottom of stack
    while(depth > 1)
    {
        top = base - 1;
        base = stack[base+1];
        depth--;
    }
    
    return (top - base + 1);
}


// Pre-Conditions:  Takes in a lexicographical level and an integer b.
// Post-Conditions: Computes and returns the AR base L levels down.

int base(int level, int b, int* stack)
{
    while(level > 0)
    {
        b = stack[b + 2];
        level--;
    }
    
    return b;
}


void fetch(struct instruction instr_array [], struct registers* reg, int* stack)
{
    
    int halt=0;
    int maxWidth=4;
    
    while(!halt)
    {
        if( (reg->pc >= MAX_CODE_LENGTH) || (reg->pc < 0) )
        {
            halt = 1;
            break;
        }
        
        // fetch the next instruction
        reg->ir = instr_array[reg->pc];
        
        // check if instruction read is beyond the program lines
        // read from input file (a marker of 0 0 0 is written there)
        if( (reg->ir.op == 0) && (reg->ir.l == 0) && (reg->ir.m == 0) )
        {
            halt = 1;
            break;
        }
        
        // display (and increment) program counter
        printf("%*d  ", maxWidth, reg->pc++);
        
        // print out operation, lexicographical level, modifier
        print_Instruction((reg->ir).op, (reg->ir).l, (reg->ir).m);
        
        // execute operation
        halt = execute(reg, stack);
        
        // display program counter, base pointer, stack pointer and stack
        printf("%*d %*d %*d", maxWidth, reg->pc, maxWidth, reg->bp, maxWidth, reg->sp);
        print_Stack(reg, stack);
        
    }
    
    return;
}


// Pre-Conditions:   reg is non-null pointer.
// Post-Conditions:  Registers initialized per program specs.

void initializeRegisters(struct registers* reg)
{
    reg->sp = 0;
    reg->bp = 1;
    reg->pc = 0;
    
    return;
}


// Pre-Conditions:  reg and stack are non-null pointers.
// Post-Conditions: Executes instruction in instruction register.

int execute(struct registers* reg, int* stack)
{
    
    switch((reg->ir).op)
    {
            // LIT: Pushes value M onto the stack
        case 1:
            
            reg->sp = reg->sp + 1;
            stack[reg->sp] = (reg->ir).m;
            break;
            
            // OPR: Performs an arithmetic or logical operation
        case 2:
            
            // RET
            if((reg->ir).m == 0)
            {
                reg->sp = (reg->bp) - 1;
                reg->pc = stack[(reg->sp) + 4];
                reg->bp = stack[(reg->sp) + 3];
                break;
            }
            
            // NEG
            if((reg->ir).m == 1)
            {
                stack[reg->sp] = -(stack[reg->sp]);
                break;
            }
            
            // ADD
            if((reg->ir).m == 2)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] + stack[(reg->sp) + 1];
                break;
            }
            
            // SUB
            if((reg->ir).m == 3)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] - stack[(reg->sp) + 1];
                break;
            }
            
            // MUL
            if((reg->ir).m == 4)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] * stack[(reg->sp) + 1];
                break;
            }
            
            // DIV
            if((reg->ir).m == 5)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] / stack[(reg->sp) + 1];
                break;
            }
            
            // ODD
            if((reg->ir).m == 6)
            {
                stack[reg->sp] = stack[reg->sp] % 2;
                break;
            }
            
            // MOD
            if((reg->ir).m == 7)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] % stack[(reg->sp) + 1];
                break;
            }
            
            // EQL
            if((reg->ir).m == 8)
            {
                reg->sp = (reg->sp) - 1;
                stack[(reg->sp)] = stack[reg->sp] == stack[(reg->sp) + 1];
                break;
            }
            
            // NEQ
            if((reg->ir).m == 9)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] != stack[(reg->sp) + 1];
                break;
            }
            
            // LSS
            if((reg->ir).m == 10)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] < stack[(reg->sp) + 1];
                break;
            }
            
            // LEQ
            if((reg->ir).m == 11)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] <= stack[(reg->sp) + 1];
                break;
            }
            
            // GTR
            if((reg->ir).m == 12)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] > stack[(reg->sp) + 1];
                break;
            }
            
            // GEQ
            if((reg->ir).m == 13)
            {
                reg->sp = (reg->sp) - 1;
                stack[reg->sp] = stack[reg->sp] >= stack[(reg->sp) + 1];
                break;
            }
            
            // LOD: Get value at offset M in frame L levels down and push it.
        case 3:
            
            reg->sp = (reg->sp) + 1;
            stack[reg->sp] = stack[base(reg->ir.l, reg->bp, stack) + reg->ir.m];
            break;
            
            // STO: Pop  and insert value at offset M in frame L levels down.
        case 4:
            
            stack[base(reg->ir.l, reg->bp, stack) + reg->ir.m] = stack[reg->sp];
            reg->sp = (reg->sp) - 1;
            break;
            
            // CAL: Call procedure at M (generates new stack frame)
        case 5:
            
            stack[(reg->sp)+1] = 0;
            stack[(reg->sp)+2] = base((reg->ir).l, reg->bp, stack);
            stack[(reg->sp)+3] = reg->bp;
            stack[(reg->sp)+4] = reg->pc;
            reg->bp = (reg->sp) + 1;
            reg->pc = (reg->ir).m;
            break;
            
            // INC: Allocate M locals on stack
        case 6:
            
            reg->sp = (reg->sp) + (reg->ir).m;
            break;
            
            // JMP: Jump to M
        case 7:
            
            reg->pc = (reg->ir).m;
            break;
            
            // JPC: Pop stack and jump to M if value is equal to 0
        case 8:
            
            if(stack[reg->sp] == 0)
            {
                reg->pc = (reg->ir).m;
            }
            
            reg->sp = (reg->sp) - 1;
            break;
            
            // SIO
        case 9:
            
            // OUT: Pop stack and print out value
            if((reg->ir).m == 0)
            {
                printf("%d", stack[reg->sp]);
                reg->sp = (reg->sp) - 1;
            }
            
            // INP: Read input from user and push it
            if((reg->ir).m == 1)
            {
                reg->sp = (reg->sp) + 1;
                scanf("%d", &(stack[reg->sp]));
            }
            
            // HLT: Halt the machine
            if((reg->ir).m == 2)
            {
                
                return 1;
            }
            
    } // END switch
    
    return 0;
}


// Pre-condition:  Legal op(eration) and m(odifier) codes passed.
// Post-condition: Prints instruction to screen.

void print_Instruction(int op, int l, int m)
{
    int maxWidth = 4;
    
    // operation
    if(op == 2)
    {
        printf("%s ", opNamesOPR[m]);
    }
    else if(op == 9)
    {
        printf("%s ", opNamesSIO[m]);
    }
    else
    {
        printf("%s ", opNames[op]);
    }
    
    // level
    if( (op >= 3) && (op <= 5) )
    {
        printf("%*d ", maxWidth, l);
    }
    else
    {
        printf("%*s ", maxWidth, "");
    }
    
    // modifier
    if( (op != 2) && (op != 9) )
    {
        printf("%*d ", maxWidth, m);
    }
    else
    {
        printf("%*s ", maxWidth, "");
    }
    
    return;
}


// Pre-Conditions:  instr_array is non-null pointer.
// Post-Conditions: Prints the program code to the screen.

void print_Program(struct instruction* instr_array, char* filename){
    
    int i = 0;
    
    printf("%s\n", filename);
    printf("PL/0 code:\n\n");
    
    while(instr_array[i].op > 0)
    {
        printf("%*d  ", 3, i);
        print_Instruction(instr_array[i].op, instr_array[i].l, instr_array[i].m);
        printf(" \n");
        i++;
    }
    
    printf("\n");
    
    return;
}


// Pre-Conditions:  reg and stack are non-null pointers.
// Post-Conditions: Prints the contents of the stack to the screen.

void print_Stack(struct registers* reg, int* stack)
{
    
    int i = 1;
    int height = 0;
    int level = 0;
    
    // for alignment purposes
    printf("   ");
    
    // get number of stack elements in first AR record
    height = ARheight(reg, stack, level++);
    
    while(i <= reg->sp)
    {
        printf("%d ", stack[i]);
        
        // print AR dividing bar '|'
        if( (i == height) && (i != reg->sp) )
        {
            printf("| ");
            height = i + ARheight(reg, stack, level++);
        }
        
        i++;
    }
    
    printf("\n");
    
    return;
}


// Pre-Conditions:   filename is valid input file, instr_array is non-null.
// Post-Conditions:  Reads file and stores instructions in provided array.
//                   Returns 0 on success, 1 on failure.

int readFile(char* filename, struct instruction* instr_array)
{
    int numInstructions = 0;
    int n0 = 0;
    int n1 = 0;
    int n2 = 0;
    char* mode = "r";
    
    FILE* fp = fopen(filename, mode);
    
    if(fp == NULL)
    {
        return 1;
    }
    
    // read until eof or max instructions read
    while( (numInstructions < MAX_CODE_LENGTH) && !feof(fp) )
    {
        fscanf(fp, "%d %d %d", &n0, &n1, &n2);
        instr_array[numInstructions].op = n0;
        instr_array[numInstructions].l  = n1;
        instr_array[numInstructions].m  = n2;
        numInstructions++;
    }
    
    // mark end of program code
    instr_array[numInstructions].op = 0;
    instr_array[numInstructions].l  = 0;
    instr_array[numInstructions].m  = 0;
    
    // close file
    fclose(fp);
    
    return 0;
}

// *** EOF ***
