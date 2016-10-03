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
//| * code expected final newline after every line of code,
//|   EOF should immediatly follow a newline character
//|===========================================================================

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#define MAX_IDENTIFIER_LENGTH     12
#define DFA_MATRIX_NUMBER_COLUMNS 75
#define DFA_MATRIX_NUMBER_STATES  76
#define DEAD_STATE                75

typedef enum token {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

union token_value
{
    int   number;
    char* identifier;

};

typedef struct
{
    union token_value val;
    token_type  t;

} aToken_type;

struct Options {
    int show_source;
    int show_clean;
    char filename[20];
};

// function prototypes
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



int main(int argc, char* argv[])
{
    struct Options* optns = malloc(sizeof(struct Options));
    setOptions(argc, argv, optns);

    FILE* rawFile;
    FILE* cleanFile;

    rawFile = fopen(optns->filename, "rb+");
    cleanFile = fopen("clean.pl0", "wb+");

    removeComments(rawFile, cleanFile);

    //FILE* cleanFile = fopen("/Users/cris/Dropbox/Programming Code/COP3402/code/Group_Compiler_PL0/Group_Compiler_PL0/test-lex2.txt","r");

    while (true)
    {
        aToken_type* toke = getNextToken(cleanFile);

        // halt if nullsym is returned
        if ( toke->t == 1 )
            break;

        displayToken(toke);
        freeToken(toke);
    }

    fclose(rawFile);
    fclose(cleanFile);

} // End main


//* setOptions:  reads arguments from cmd line and sorts them into
//*             Options struct.  Returns 0 on success, 1 on failure.
int  setOptions (int argc, char* argv[], struct Options* optns){

    //return 1 if too many args
    if (argc > 4){
        printf("Too many arguments. Allowed: (String) filename, \"--source\", \"--clean\"\n");
        return 1;
    }

    //start with both flags off
    optns->show_source = 0;
    optns->show_clean = 0;
    strcpy(optns->filename, "");

    //read args
    int i;
    for(i=1; i<argc; ++i){
        if (strcmp(argv[i], "--source") == 0){
            optns->show_source = 1;
        }
        else if (strcmp(argv[i], "--clean")==0){
            optns->show_clean = 1;
        }
        //only allow one non-flag arg
        else if (strcmp(optns->filename, "")==0){
            strcpy(optns->filename, argv[i]);
        }
        else{
            printf("Error: invalid arg %s.\n Allowed: (String) filename, \"--source\", \"--clean\"\n", argv[i]);
            return 1;
        }
    }
    return 0;
}

// removeComments:  reads a file and removes /*comments*/, preserving
//                  whitespace. Returns 0 on success, 1 on failure.
int  removeComments (FILE* inFile, FILE* cleanFile){

    //check validity of file pointers
    if (inFile == NULL ){
        printf("Invalid input file pointer\n");
        return 1;
    } else if (cleanFile == NULL){
        printf("Invalid clean output file pointer\n");
        return 1;
    }

    //tiny helper function
    void readnextc(FILE* f, char* buff){
        buff[0] = buff[1];
        char c = fgetc(f);
        if (!feof(f)){
            buff[1] = c;
        } else {
            buff[1] = ' ';
        }
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
        if ( strcmp(readbuff, "/*") == 0){
            //loop until end of comment
            do{
                readnextc(inFile, readbuff);
            }while ( strcmp(readbuff, "*/") != 0 );
            //move past comment close
            readnextc(inFile, readbuff);
            readnextc(inFile, readbuff);
        }

        //write a char
        fputc(readbuff[0], cleanFile);

    }
    return 0;
}


void displayError(int code, int var)
{

    switch (code)
    {
		case (1):

			printf("DFA: finished in dead state\n");
			break;

		case (2):

			printf("Invalid column number returned by getDFAcolumnNumber (column: %d)\n", var);
			break;

		case (3):

			printf("Identifier too long\n");
			break;

		case (4):

			printf("Number out of acceptable range (val: %d)\n", var);
			break;

		case (5):

			printf("nextState was passed an invalid current state (state: %d)\n", var);
			break;

		case (6):

			printf("nextState was passed an invalid input column (column: %d)\n", var);
			break;

		case (7):

			printf("stateToTokenTypeOrdinal was passed an invalid state (state: %d)\n", var);
			break;

		case (8):

			printf("getDFAcolumnNumber was passed an invalid character (value: %d)\n", var);
			break;

		case (10):

			printf("identifier cannot start with a number\n");
			break;

		case (11):

			printf("invalid token\n");
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

    // Read in contents until end of file is reached.
    // Display file content.
    while (!feof(ifp)){
        c = fgetc(ifp);
        printf("%c", c);
    }

    //OPTIONAL: Add space between printed statements.
    printf("\n\n");

} // END display source file


void  displayToken(aToken_type* t)
{

    const char* tokenTypeNames[] = {
        "PLACE_HOLDER",//  to align other values with enum token_type (starts at 1)
        "nulsym",      //  1
        "identsym",    //  2
        "numbersym",   //  3
        "plussym",     //  4
        "minussym",    //  5
        "multsym",     //  6
        "slashsym",    //  7
        "oddsym",      //  8
        "eqsym",       //  9
        "neqsym",      // 10
        "lessym",      // 11
        "leqsym",      // 12
        "gtrsym",      // 13
        "geqsym",      // 14
        "lparentsym",  // 15
        "rparentsym",  // 16
        "commasym",    // 17
        "semicolonsym",// 18
        "periodsym",   // 19
        "becomessym",  // 20
        "beginsym",    // 21
        "endsym",      // 22
        "ifsym",       // 23
        "thensym",     // 24
        "whilesym",    // 25
        "dosym",       // 26
        "callsym",     // 27
        "constsym",    // 28
        "varsym",      // 29
        "procsym",     // 30
        "writesym",    // 31
        "readsym",     // 32
        "elsesym" };   // 33

    printf("<%s> ", tokenTypeNames[t->t]);

    if      (t->t == 2)
    {
        printf("value: %s", t->val.identifier);
    }
    else if (t->t == 3)
    {
        printf("value: %d", t->val.number);
    }

    printf("\n");

    return;

} // END display token


void  freeToken(aToken_type* t)
{

    if (t != NULL)
    {
        if (t->t == 2)
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

    // translate A-Z to column number (can combine?)
    if ( (c >= 65) && (c <= 90) )
    {
         DFAcolumnNumber = c - 'A' + 0;
    }
    // else translate a-Z to column number (can combine?)
    else if ( (c >= 97) && (c <= 122) )
    {
         DFAcolumnNumber = c - 'a' + 26;
    }
    // else translate 0-9 to column number (can combine?)
    else if ( (c >= 48) && (c <= 57) )
    {
         DFAcolumnNumber = c - '0' + 52;
    }
    // else translate allowed chars to column number....
    else if ( c == '+' )
    {
        DFAcolumnNumber = 62;
    }
    else if ( c == '-' )
    {
        DFAcolumnNumber = 63;
    }
    else if ( c == '*' )
    {
        DFAcolumnNumber = 64;
    }
    else if ( c == '/' )
    {
        DFAcolumnNumber = 65;
    }
    else if ( c == '=' )
    {
        DFAcolumnNumber = 66;
    }
    else if ( c == '<' )
    {
        DFAcolumnNumber = 67;
    }
    else if ( c == '>' )
    {
        DFAcolumnNumber = 68;
    }
    else if ( c == ':' )
    {
        DFAcolumnNumber = 69;
    }
    else if ( c == ';' )
    {
        DFAcolumnNumber = 70;
    }
    else if ( c == ',' )
    {
        DFAcolumnNumber = 71;
    }
    else if ( c == '.' )
    {
        DFAcolumnNumber = 72;
    }
    else if ( c == '(' )
    {
        DFAcolumnNumber = 73;
    }
    else if ( c == ')' )
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

    while (true)
    {
        moveFilePointerOneBack = true;

        fscanf(cleanFile, "%c", &c);

        // loop if on first char of lexeme until a non-ignored character is read in (break for EOF)
                                /*   tab         cr           lf          space   */
        while ( (length == 0) && ((c == 9) || (c == 13) || (c == 10) || (c == 32)) && !feof(cleanFile))
        {
            fscanf(cleanFile, "%c", &c);
        }

        // handle eof-only lexeme
        if (feof(cleanFile) && (length == 0))
        {
            DFAstate = 0;
            break;
        }

        // handle ignored characters following non-null lexeme
                            /*   tab         cr           lf          space   */
        if ( (length > 0) && ((c == 9) || (c == 13) || (c == 10) || (c == 32)) )
        {
            moveFilePointerOneBack = false;
            break;
        }

        // get DFA column number
        DFAcolumn = getDFAcolumnNumber(c);

        // stop/error on invalid column number ... BUT NOT EOF!!!
        if ( DFAcolumn > DFA_MATRIX_NUMBER_COLUMNS )
        {
            displayError(2,DFAcolumn);
            break;
        }

        // get next DFA state
        DFAstate_prev = DFAstate;
        DFAstate = nextState(DFAstate, DFAcolumn);

        // check if (current) state is dead, previously was a number, and input was letter
        if ( (DFAstate == DEAD_STATE) && (DFAstate_prev == 2) && (DFAcolumn <= 51) )
        {
            DFAstate = 0; // signal halt to caller
            displayError(10, 0);
            break;
        }

        // check if (current) state is dead
        if ( DFAstate == DEAD_STATE )
        {
            // roll back state
            DFAstate = DFAstate_prev;
            moveFilePointerOneBack = true;
            break;
        }

        // if we aren't dead, is this an identifier that's too long?
        if ( (length >= MAX_IDENTIFIER_LENGTH) && (DFAstate == 1) )
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

    if ( moveFilePointerOneBack )
    {
        fseek(cleanFile, -1, SEEK_CUR);
    }

    // dead state
    if ( DFAstate == DEAD_STATE )
    {
        displayError(1,0);
        return t;
    }

    //  convert state to token type
    tokenTypeOrdinal = stateToTokenTypeOrdinal(DFAstate);
    t->t = tokenTypeOrdinal;

    switch (tokenTypeOrdinal)
    {

    // invalid token type, invalid character (e.g. ':' without '=', or '$', or '@', etc)
    case (0):

            displayError(11, 0);
            t->t = 1; // signal halt to caller
            free(lexeme);
            break;

    // identifier token -- lexeme must live
    case (2):

            t->val.identifier = lexeme;
            break;

    // number token
    case (3):

            sscanf(lexeme, "%d", &i);
            if ( (i > 32767) || (i < -32768) )
            {
                t->val.number = 1; // signal halt to caller
                DFAstate      = 0; // signal halt to caller
                displayError(4, i);
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

    return t;

} // END get next token


int nextState(int currentState, int input)
{

    if ( (currentState <0) || (currentState >= DFA_MATRIX_NUMBER_STATES) )
    {
        displayError(5, currentState);
        return DEAD_STATE;
    }

    if ( (input < 0) || (input >= DFA_MATRIX_NUMBER_COLUMNS) )
    {
        displayError(6, input);
        return DEAD_STATE;
    }

    /* DFA */
    int DFA[76][75] = {
                /*      0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74   */
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


// will return 0 on invalid character
int stateToTokenTypeOrdinal(int s)
{

    if ( (s < 0 ) || ( s >= DFA_MATRIX_NUMBER_STATES) )
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
