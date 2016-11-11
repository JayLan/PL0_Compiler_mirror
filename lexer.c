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
//|===========================================================================

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "tokens.h"

#define MAX_IDENTIFIER_LENGTH     12
#define DFA_MATRIX_NUMBER_COLUMNS 75
#define DFA_MATRIX_NUMBER_STATES  76
#define DEAD_STATE                75

/*typedef enum token {
    nulsym = 1, identsym, numbersym, plussym, minussym,
    multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
    gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
    periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
    readsym , elsesym
} token_type;


//Token_Value can be either an integer or a string
union token_value
{
    int   number;
    char* identifier;

};

//aToken_Type stores the kind of value, and the actual token.
typedef struct
{
    union token_value val;
    token_type  t;

} aToken_type;
*/
struct Options {
    bool show_source;
    bool show_clean;
    char filename[255];
};

// function prototypes
int do_lex(FILE* clean);
aToken_type* getNextToken(FILE* cleanFile);
int  getDFAcolumnNumber (char c);
int  nextState(int currentState, int input);
int  stateToTokenTypeOrdinal(int s);
int  setOptions (int argc, char* argv[], struct Options* Options);
int  removeComments (FILE* infile, FILE* cleanFile);
void displayError(int code, int var);
void displaySourceFile(FILE* ifp);
void displayToken(aToken_type* t);
void freeToken(aToken_type* t);
void readnextc(FILE* f, char* buff);


/*
int main(int argc, char* argv[])
{
    //Allocates memory for the flags and sends them to setOptions:
    struct Options* optns = malloc(sizeof(struct Options));
    setOptions(argc, argv, optns);

    //Declare and Initialize File Pointers
    FILE* rawFile;
    FILE* cleanFile;

    rawFile = fopen(optns->filename, "rb+");
    cleanFile = fopen("clean.pl0", "wb+");
*/
    /* display original code, if desired */
    /*if(optns->show_source == true)
    {
        printf("\n");
        printf("source code:\n");
        printf("------------\n");
        displaySourceFile(rawFile);
    }

    removeComments(rawFile, cleanFile);

    fclose(rawFile);
    fclose(cleanFile);
*/
    //Remove Comments from the original source file,
    //then close both file pointers:

    //Re-initialize the clean file pointer:
    //cleanFile = fopen("clean.pl0", "rb+");

    /* display comment-free code, if desired */
    /*if(optns->show_clean == true)
    {
        printf("\n");
        printf("source code without comments:\n");
        printf("-----------------------------\n");
        displaySourceFile(cleanFile);
    }
*/
int do_lex(FILE* cleanFile){

    printf("\n");
    printf("tokens:\n");
    printf("-------\n");

    while(true)
    {
        //Get the tokens from the clean file.
        aToken_type* toke = getNextToken(cleanFile);

        //this is where we will send the token to tokArr in tokens.h
        addToken(toke);

        //printf("do_lex: toke->t = %d\n", toke->t);

        // halt if nullsym is returned
        if ( toke->t == 1 ){
            //printf("do_lex: breaking after nullsym found\n", toke->t);
            break;
        }

        //Display the appropriate token, then free it
        //displayToken(toke); --token will be displayed in by the parser or driver
        freeToken(toke);
    }

    //Close the cleanFile pointer
    fclose(cleanFile);


} // End main

//Takes in an integer identifying what kind of error is found.
//Displays the appropriate error to the user
void displayError(int code, int var)
{

    switch(code)
    {
        case(1):

            printf("DFA: finished in dead state\n");
            break;

        case(2):

            printf("Invalid column number returned by getDFAcolumnNumber (column: %d)\n", var);
            break;

        case(3):

            // this error message required by assignment specs
            printf("Identifier too long\n");
            break;

        case(4):

            // this error message required by assignment specs
            printf("Number out of acceptable range (val: %d)\n", var);
            break;

        case(5):

            printf("nextState was passed an invalid current state (state: %d)\n", var);
            break;

        case(6):

            printf("nextState was passed an invalid input column (column: %d)\n", var);
            break;

        case(7):

            printf("stateToTokenTypeOrdinal was passed an invalid state (state: %d)\n", var);
            break;

        case(8):

            printf("getDFAcolumnNumber was passed an invalid character (value: %d)\n", var);
            break;

        case(10):

            // this error message required by assignment specs
            printf("Identifier cannot start with a number\n");
            break;

        case(11):

            // this error message required by assignment specs
            printf("Invalid token\n");
            break;

        case(12):
            printf("displaySourceFile invalid file pointer\n");
            break;


        case(13):
            printf("removeComments invalid input file pointer\n");
            break;

        case(14):

            printf("removeComments invalid clean output file pointer\n");
            break;

        case(15):

            printf("Too many arguments. Allowed: (String) filename, \"--source\", \"--clean\"\n");
            break;

        case(16):

            printf("Error: invalid arg. Allowed: (String) filename, \"--source\", \"--clean\"\n");
            break;

        default:

            printf("unknown error occured\n");
            break;
    }

} // END display error


// Pre-Conditions: Takes in a valid index file pointer.
// Post-Conditions: Prints the contents of the file to the user.
void displaySourceFile(FILE* ifp){

    char c;

    //Check to make sure File Pointer is not NULL.
    //If NULL then display the error and return from function.
    if (ifp == NULL)
    {
        //displayError(12, 0);
        return;
    }

    // Read in contents until end of file is reached.
    // Display file content.
    while(true){
        c = fgetc(ifp);
        if (!feof(ifp))
        {
            printf("%c", c);
        }
        else
        {
            break;
        }
    }

    // movie file pointer back to start of file
    fseek(ifp, 0, SEEK_SET);

} // END display source file

//Takes in a valid aToken_type pointer t that contains the kind of token.
void  displayToken(aToken_type* t)
{
    //Declare an array with possible token types.
    const char* tokenTypeNames[] = {
        "PLACE_HOLDER",//  to align other values with enum token_type (starts at 1)
        "nulsym",   //  1
        "identsym", //  2
        "numbersym",//  3
        "+",        //  4
        "-",        //  5
        "*",        //  6
        "/",        //  7
        "odd",      //  8
        "=",        //  9
        "<>",       // 10
        "<",        // 11
        "<=",       // 12
        ">",        // 13
        ">=",       // 14
        "(",        // 15
        ")",        // 16
        ",",        // 17
        ";",        // 18
        ".",        // 19
        ":=",       // 20
        "begin",    // 21
        "end",      // 22
        "if",       // 23
        "then",     // 24
        "while",    // 25
        "do",       // 26
        "call",     // 27
        "const",    // 28
        "var",      // 29
        "procedure",// 30
        "write",    // 31
        "read",     // 32
        "else" };   // 33

    /*if(t->t == 2)
    {
        printf("%-13s", t->val.identifier);
    }
    else if(t->t == 3)
    {
        printf("%-13d", t->val.number);
    }
    else
    {
        printf("%-13s", tokenTypeNames[t->t]);
    }

    printf("%d\n", t->t);

    return;
*/

printf("%d ", t->t);

    if(t->t == 2)
    {
        printf("%s ", t->val.identifier);
    }
    else if(t->t == 3)
    {
        printf("%d ", t->val.number);
    }


    return;

} // END display token


//Frees the memory taken up by aToken_type pointer t
void  freeToken(aToken_type* t)
{

    if(t != NULL)
    {
        if(t->t == 2)
        {
            free(t->val.identifier);
        }

        free(t);
    }

    return;

} // END free token


int getDFAcolumnNumber(char c)
{

    int DFAcolumnNumber = 0;

    // A-Z
    if( (c >= 65) && (c <= 90) )
    {
        DFAcolumnNumber = c - 'A' + 0;
    }
    // a-Z
    else if( (c >= 97) && (c <= 122) )
    {
        DFAcolumnNumber = c - 'a' + 26;
    }
    // 0-9
    else if( (c >= 48) && (c <= 57) )
    {
        DFAcolumnNumber = c - '0' + 52;
    }
    // lastly, legal characters...
    else if( c == '+' )
    {
        DFAcolumnNumber = 62;
    }
    else if( c == '-' )
    {
        DFAcolumnNumber = 63;
    }
    else if( c == '*' )
    {
        DFAcolumnNumber = 64;
    }
    else if( c == '/' )
    {
        DFAcolumnNumber = 65;
    }
    else if( c == '=' )
    {
        DFAcolumnNumber = 66;
    }
    else if( c == '<' )
    {
        DFAcolumnNumber = 67;
    }
    else if( c == '>' )
    {
        DFAcolumnNumber = 68;
    }
    else if( c == ':' )
    {
        DFAcolumnNumber = 69;
    }
    else if( c == ';' )
    {
        DFAcolumnNumber = 70;
    }
    else if( c == ',' )
    {
        DFAcolumnNumber = 71;
    }
    else if( c == '.' )
    {
        DFAcolumnNumber = 72;
    }
    else if( c == '(' )
    {
        DFAcolumnNumber = 73;
    }
    else if( c == ')' )
    {
        DFAcolumnNumber = 74;
    }

    // else fail invlaid input charater
    else
    {
        displayError(8, c);
    }

    return DFAcolumnNumber;

} // END get DFA column number


//  returns next token starting at current position of file pointer
aToken_type* getNextToken(FILE* cleanFile)
{

    bool moveFilePointerOneBack = true;

    int length         = 0;
    int DFAstate       = 0; // 0 is start state
    int DFAstate_prev  = 0;
    int DFAcolumn      = DFA_MATRIX_NUMBER_COLUMNS+1;
    int i              = 0;
    int tokenTypeOrdinal = 0;

    // longest identifier is 12 but a number with many leading 0's could be valid
    // ! things could go BAD if a number longer than 254 digits long is read !
    char* lexeme = calloc(255, sizeof(char));
    char c = 9; // default to tab

    aToken_type* t = (aToken_type*)malloc(sizeof(aToken_type));

    // return nullsym token on eof
    if (feof(cleanFile))
    {
        t->t = 1;
        t->t = 1;
        return t;
    }

    while(true)
    {
        moveFilePointerOneBack = true;

        fscanf(cleanFile, "%c", &c);

        if( feof(cleanFile) )
        {
            moveFilePointerOneBack = false;
            break;
        }

        /* loop (if on first char of lexeme) until a non-ignored character is read in (break for EOF  */
        /*                          tab         cr           lf          space                        */
        while( (length == 0) && ((c == 9) || (c == 13) || (c == 10) || (c == 32)) && !feof(cleanFile) )
        {
            fscanf(cleanFile, "%c", &c);
        }

        // handle eof-only lexeme
        if(feof(cleanFile) && (length == 0))
        {
            DFAstate = 0;
            break;
        }

        /* handle ignored characters following non-null lexeme               */
        /*                     tab         cr           lf          space    */
        if( (length > 0) && ((c == 9) || (c == 13) || (c == 10) || (c == 32)) )
        {
            moveFilePointerOneBack = false;
            break;
        }

        // get DFA column number
        DFAcolumn = getDFAcolumnNumber(c);

        // stop/error on invalid column number ... BUT NOT EOF!!!
        if( DFAcolumn > DFA_MATRIX_NUMBER_COLUMNS )
        {
            displayError(2,DFAcolumn);
            break;
        }

        // get next DFA state
        DFAstate_prev = DFAstate;
        DFAstate = nextState(DFAstate, DFAcolumn);

        // check if (current) state is dead, previously was a number, and input was letter
        if( (DFAstate == DEAD_STATE) && (DFAstate_prev == 2) && (DFAcolumn <= 51) )
        {
            DFAstate = 0; // signal halt to caller
            displayError(10, 0);
            break;
        }

        // check if (current) state is dead
        if( DFAstate == DEAD_STATE )
        {
            // roll back state
            moveFilePointerOneBack = true;
            DFAstate = DFAstate_prev;
            break;
        }

        // if we aren't dead, is this an identifier that's too long?
        if( (length >= MAX_IDENTIFIER_LENGTH) && (DFAstate == 1) )
        {
            DFAstate = 0; // signal halt to caller
            displayError(3, 0);
            break;
        }

        // since we aren't dead, append neweest char onto lexeme
        lexeme[length] = c;

        ++length;
    }

    lexeme[length] = '\0';

    if( moveFilePointerOneBack )
    {
        fseek(cleanFile, -1, SEEK_CUR);
    }

    // dead state
    if( DFAstate == DEAD_STATE )
    {
        displayError(1,0);
        return t;
    }

    //  convert state to token type
    tokenTypeOrdinal = stateToTokenTypeOrdinal(DFAstate);
    t->t = tokenTypeOrdinal;

    switch(tokenTypeOrdinal)
    {

            // invalid token type, invalid character (e.g. ':' without '=', or '$', or '@', etc)
        case(0):

            displayError(11, 0);
            t->t = 1; // signal halt to caller
            free(lexeme);
            break;

            // identifier token -- lexeme must live
        case(2):

            t->val.identifier = lexeme;
            break;

            // number token
        case(3):

            sscanf(lexeme, "%d", &i);
            if ( (i > 32767) || (i < -32768) )
            {
                // display error and signal halt
                t->t          = 1;
                DFAstate      = 0;
                displayError(4, i);
                break;
            }
            else
            {
                t->val.number = i;
            }

            free(lexeme);
            break;

            // case - symbol token (no other values need be stored)
        default:

            free(lexeme);
            break;
    }

    //printf("added a token %d\n", t->t);
    return t;

} // END get next token


int nextState(int currentState, int input)
{

    if( (currentState <0) || (currentState >= DFA_MATRIX_NUMBER_STATES) )
    {
        displayError(5, currentState);
        return DEAD_STATE;
    }

    if( (input < 0) || (input >= DFA_MATRIX_NUMBER_COLUMNS) )
    {
        displayError(6, input);
        return DEAD_STATE;
    }

    /* DFA */
    int DFA[76][75] = {
        /*                  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74   */
        /*      A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o   p   q   r   s   t   u   v   w   x   y   z   0   1   2   3   4   5   6   7   8   9   +   -   *   /   =   <   >   :   ;   ,   .   (   )    */
        /* 0 START  */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  3,  8,  16, 18, 1,  1,  1,  24, 1,  1,  1,  1,  1,  26, 29, 1,  38, 1,  42, 1,  46, 49, 1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  58, 59, 60, 61, 62, 63, 66, 68, 70, 71, 72, 73, 74  },
        /*f1 ident  */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f2 number */  {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 3 b     */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  4,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 4 be    */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  5,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 5 beg   */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  6,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 6 begi  */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  7,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f7 begin */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 8 c      */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  13, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  9,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 9 co     */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  10, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 10 con   */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  11, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 11 cons  */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  12, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f12 const */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 13 ca   */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  14, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 14 cal  */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  15, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f15 call */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 16 d  */     {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  17, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f17 do */     {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 18 e    */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  19, 1,  22, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 19 el   */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  20, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 20 els  */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  21, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f21 else */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 22 en  */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  23, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f23 end */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 24 i  */     {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  25, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f25 if */     {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 26 o   */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  27, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 27 od  */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  28, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f28 odd */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 29 p         */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  30, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 30 pr        */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  31, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 31 pro       */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  32, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 32 proc      */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  33, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 33 proce     */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  34, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 34 proced    */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  35, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 35 procedu   */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  36, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 36 procedur  */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  37, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f37 procedure */ {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 38 r    */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  39, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 39 re   */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  40, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 40 rea  */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  41, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f41 read */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 42 t    */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  43, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 43 th   */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  44, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 44 the  */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  45, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f45 then */   {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 46 v   */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  47, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 47 va  */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  48, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f48 var */    {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 49 w     */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  50, 1,  1,  1,  1,  1,  1,  1,  1,  1,  54, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 50 wh    */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  51, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 51 whi   */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  52, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 52 whil  */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  53, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f53 while */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 54 wr    */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  55, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 55 wri   */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  56, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 56 writ  */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  57,  1, 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f57 write */  {   1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f58 + */      {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f59 - */      {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f60 * */      {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f61 / */      {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f62 = */      {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f63 < */      {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 65, 75, 64, 75, 75, 75, 75, 75, 75  },
        /*f64 <> */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f65 <= */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f66 >  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 67, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f67 >= */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 68 :  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 69, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f69 := */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f70 ;  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f71 ,  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f72 .  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f73 (  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /*f74 )  */     {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  },
        /* 75 DEAD */   {   75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75  } };

    return DFA[currentState][input];

} // END next state


//tiny helper function that reads in the next character using the buffer:
void readnextc(FILE* f, char* buff){
    buff[0] = buff[1];
    char c = fgetc(f);

    if(!feof(f)){
        buff[1] = c;
    } else {
        buff[1] = ' ';
    }

}


// removeComments:  reads a file and removes /*comments*/, preserving
//                  whitespace. Returns 0 on success, 1 on failure.
int removeComments(FILE* inFile, FILE* cleanFile){

    //check validity of file pointers
    if(inFile == NULL ){
        displayError(13, 0);
        return 1;
    } else if(cleanFile == NULL){
        displayError(14, 0);
        return 1;
    }

    //create a buffer to read two char at a time
    char readbuff[3];
    readbuff[0] = '\0'; readbuff[1] = '\0'; readbuff[2] = '\0';

    //start reading/copying
    readnextc(inFile, readbuff);
    while( !feof(inFile) ){

        //read a char
        readnextc(inFile, readbuff);

        //check for start of comment
        if( strcmp(readbuff, "/*") == 0){
            //loop until end of comment
            do{
                readnextc(inFile, readbuff);
            }while( strcmp(readbuff, "*/") != 0 );
            //move past comment close
            readnextc(inFile, readbuff);
            readnextc(inFile, readbuff);
        }

        //write a char
        fputc(readbuff[0], cleanFile);

    }
    return 0;

}


// setOptions: reads arguments from cmd line and sorts them into
//             Options struct.  Returns 0 on success, 1 on failure.
/*int setOptions(int argc, char* argv[], struct Options* optns){

    //return 1 if too many args
    if(argc > 4){
        displayError(15, argc);
        return 1;
    }

    //start with both flags off
    optns->show_source = false;
    optns->show_clean = false;
    strcpy(optns->filename, "");

    //read args
    int i;
    for(i=1; i<argc; ++i){

        //If the argument is "--source", set show_source to true.
        if(strcmp(argv[i], "--source") == 0){
            optns->show_source = true;
        }

        //If argument is "--clean" set show_source to true
        else if(strcmp(argv[i], "--clean") == 0){
            optns->show_clean =  true;;
        }
        //only allow one non-flag arg
        else if(strcmp(optns->filename, "") == 0){
            strcpy(optns->filename, argv[i]);
        }
        else{
            displayError(16, 0);
            return true;
        }
    }
    return false;

} // END set options

*/
// will return 0 on invalid character
int stateToTokenTypeOrdinal(int s)
{

    if( (s < 0 ) || ( s >= DFA_MATRIX_NUMBER_STATES) )
    {
        displayError(7, s);
        return 0;
    }

    int ordinalTable[76] = {
        /* 0 START  */      1,
        /*f1 ident  */      2,
        /*f2 number */      3,
        /* 3 b     */       2,
        /* 4 be    */       2,
        /* 5 beg   */       2,
        /* 6 begi  */       2,
        /*f7 begin */       21,
        /* 8 c      */      2,
        /* 9 co     */      2,
        /* 10 con   */      2,
        /* 11 cons  */      2,
        /*f12 const */      28,
        /* 13 ca   */       2,
        /* 14 cal  */       2,
        /*f15 call */       27,
        /* 16 d  */         2,
        /*f17 do */         26,
        /* 18 e    */       2,
        /* 19 el   */       2,
        /* 20 els  */       2,
        /*f21 else */       33,
        /* 22 en  */        2,
        /*f23 end */        22,
        /* 24 i  */         2,
        /*f25 if */         23,
        /* 26 o   */        2,
        /* 27 od  */        2,
        /*f28 odd */        8,
        /* 29 p         */  2,
        /* 30 pr        */  2,
        /* 31 pro       */  2,
        /* 32 proc      */  2,
        /* 33 proce     */  2,
        /* 34 proced    */  2,
        /* 35 procedu   */  2,
        /* 36 procedur  */  2,
        /*f37 procedure */  30,
        /* 38 r    */       2,
        /* 39 re   */       2,
        /* 40 rea  */       2,
        /*f41 read */       32,
        /* 42 t    */       2,
        /* 43 th   */       2,
        /* 44 the  */       2,
        /*f45 then */       24,
        /* 46 v   */        2,
        /* 47 va  */        2,
        /*f48 var */        29,
        /* 49 w     */      2,
        /* 50 wh    */      2,
        /* 51 whi   */      2,
        /* 52 whil  */      2,
        /*f53 while */      25,
        /* 54 wr    */      2,
        /* 55 wri   */      2,
        /* 56 writ  */      2,
        /*f57 write */      31,
        /*f58 + */          4,
        /*f59 - */          5,
        /*f60 * */          6,
        /*f61 / */          7,
        /*f62 = */          9,
        /*f63 < */          11,
        /*f64 <> */         10,
        /*f65 <= */         12,
        /*f66 >  */         13,
        /*f67 >= */         14,
        /* 68 :  */         0,  // will use 0 to represent invalid character/token
        /*f69 := */         20, // double check (is this becomessym?)
        /*f70 ;  */         18,
        /*f71 ,  */         17,
        /*f72 .  */         19,
        /*f73 (  */         15,
        /*f74 )  */         16,
        /* 75 DEAD */       1};

    return ordinalTable[s];

} // END state to token type ordinal



