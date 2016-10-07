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

#define MAX_IDENTIFIER_LENGTH     12
#define DFA_MATRIX_NUMBER_COLUMNS 127
#define DEAD_STATE                0
#define START_STATE               1
#define IDENT_STATE               2
#define NUMBER_STATE              3

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
    bool show_source;
    bool show_clean;
    char filename[255];
};

typedef struct DFA {
    char** tokenNames;
    int** matrix;
    int* rowToTokenType;
    int numMatrixRows;
} DFA_type;

// function prototypes
aToken_type* getNextToken(FILE* cleanFile, DFA_type* DFA);
bool isLetter(int c);
bool isIgnoredChar(int c);
DFA_type* DFAmaker(char** words, size_t words_len, int* words_token,
                   char** symbols, size_t symbols_len, int* symbols_token);
int  setOptions (int argc, char* argv[], struct Options* Options);
int  removeComments (FILE* infile, FILE* cleanFile);
void bubbleConSort(char** list, int* indices, size_t len, int i);
void displayError(int code, int var);
void displaySourceFile(FILE* ifp);
void displayToken(aToken_type* t, const char* tokenNames[]);
void freeToken(aToken_type* t);
void readnextc(FILE* f, char* buff);
void print_cstring_array(char **array, size_t len);


// functions
int main(int argc, char* argv[])
{
    
    struct Options* optns = malloc(sizeof(struct Options));
    setOptions(argc, argv, optns);
    
    FILE* rawFile;
    FILE* cleanFile;
    
    rawFile = fopen(optns->filename, "rb+");
    cleanFile = fopen("clean.pl0", "wb+");
    
    /* display original code, if desired */
    if(optns->show_source == true)
    {
        printf("\n");
        printf("source code:\n");
        printf("------------\n");
        displaySourceFile(rawFile);
    }
    
    removeComments(rawFile, cleanFile);
    
    fclose(rawFile);
    fclose(cleanFile);
    
    cleanFile = fopen("clean.pl0", "rb+");
    
    /* display comment-free code, if desired */
    if(optns->show_clean == true)
    {
        printf("\n");
        printf("source code without comments:\n");
        printf("-----------------------------\n");
        displaySourceFile(cleanFile);
    }
    
    /* setup DFA */
    char* words[]     = {"const","var","procedure","call","begin","end","if","then","while","do","read","write","odd"};
    int words_token[] = {     28,   29,         30,    27,     21,   22,  23,    24,    25,   26,    32,     31,   8 };
    size_t words_len = sizeof(words) / sizeof(char *);
    
    char* syms[]     = {"+","-","*","/","=","<>","<=","<",">=",">",":=",",",";",".","(",")"};
    int syms_token[] = {  4,  5,  6,  7,  9,  10,  12, 11,  14, 13,  20, 17, 18, 19, 15, 16};
    size_t syms_len = sizeof(syms) / sizeof(char *);
    
    const char* tokenNames[] = {
        "PLACE_HOLDER", "nulsym", "identsym", "numbersym", "+", "-", "*", "/",
        "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=",
        "begin", "end", "if", "then", "while", "do", "call", "const", "var",
        "procedure", "write", "read", "else" };
    
    DFA_type* DFA = DFAmaker(words, words_len, words_token, syms, syms_len, syms_token);
    
    printf("\n");
    printf("tokens:\n");
    printf("-------\n");
    
    /* lex */
    while(true)
    {
        aToken_type* toke = getNextToken(cleanFile, DFA);
        
        // halt if nullsym is returned
        if ( toke->t == 1 )
            break;
        
        displayToken(toke, tokenNames);
        freeToken(toke);
    }
    
    
    /* clean up */
    fclose(cleanFile);
    
} // End main


// bubble sorts one array based on its values (strcmp) and simultaneously reorders other array using same pattern
void bubbleConSort(char** list, int* indices, size_t len, int x)
{
    if (x<1 || x>2)
    {
        printf("invalid index of which array to sort by (1 or 2 only)\n");
        return;
    }
    
    for (int i=0; i<len; i++)
    {
        for (int j=i+1; j<len; j++)
        {
            if ( ((x==1) && (strcmp(list[i],list[j]) > 0)) ||
                ((x==2) && (indices[i] > indices[j])) )
            {
                char* t = list[i];
                list[i] = list[j];
                list[j] = t;
                
                int s    = indices[i];
                indices[i] = indices[j];
                indices[j] = s;
            }
        }
    }
}


// creates DFA matrix and state-to-token table from arrays of words, symbols and corresponding token numbers
DFA_type* DFAmaker(char** words, size_t words_len, int* words_token, char** symbols, size_t symbols_len, int* symbols_token)
{
    bool ignoreCharacter = false;
    
    const int NUM_OF_INPUT_CHARS = 127;
    const int FIRST_NUMBER    = 48;
    const int  LAST_NUMBER    = 57;
    const int FIRST_LOWERCASE = 97;
    const int  LAST_LOWERCASE = 122;
    const int FIRST_UPPERCASE = 65;
    const int  LAST_UPPERCASE = 90;
    
    int numDFArows=4; // dead, start, ident, number
    int i=0, j=0, currentRow=0;
    
    DFA_type* DFA = malloc(sizeof(DFA_type));
    
    // sort word and symbol lists alphabetically
    bubbleConSort(words, words_token, words_len, 1);
    bubbleConSort(symbols, symbols_token, symbols_len, 1);
    
    // calculate number of DFA rows to accomodate (partial/complete) reserved word states
    i=0;
    ignoreCharacter = false; // first word only, default to false
    while ( i<words_len )
    {
        j=0;
        
        while ( words[i][j] != '\0' )
        {
            // stays true for remainder of a word after first trip, reset when next word begins
            if ( (i>0) && (words[i-1][j] !=  words[i][j]))
                ignoreCharacter = false;
            
            if ( !ignoreCharacter )
                ++numDFArows;
            
            ++j;
        }
        ++i;
        ignoreCharacter = true; // except for first word, default to true
    }
    
    // calculate number of DFA rows to accomodate (partial/complete) symbol states
    i=0;
    ignoreCharacter = false;
    while ( i<symbols_len )
    {
        
        j=0;
        
        while ( symbols[i][j] != '\0' )
        {
            // stays true for remainder of a symbol after first trip, reset when next word begins
            if ( (i>0) && (symbols[i-1][j] !=  symbols[i][j]) )
                ignoreCharacter = false;
            
            if ( !ignoreCharacter )
                ++numDFArows;
            
            ++j;
        }
        i++;
        ignoreCharacter = true;
    }
    
    DFA->numMatrixRows = numDFArows;
    
    // this is for translate final matrix row -> token type
    DFA->rowToTokenType = calloc(numDFArows, sizeof(int));
    
    DFA->matrix = malloc(sizeof(int*)*numDFArows);
    for (i=0; i<numDFArows; i++)
        DFA->matrix[i] = calloc(NUM_OF_INPUT_CHARS,sizeof(int)); // chars 0 through 126 are inputs in DFA matrix
    
    // init matrix dead row (row 0)
    // NOTE: dead row already init by calloc
    ++currentRow;
    
    // init start row (row 1)
    // NOTE: start row init w/ ident state for all letters, number state for numbers, dead for all others
    //       ... the 'all others' are already init by calloc (to DEAD_STATE)
    for (i=FIRST_NUMBER; i<=LAST_NUMBER; i++)
        DFA->matrix[currentRow][i] = NUMBER_STATE;
    
    for (i=FIRST_LOWERCASE; i<=LAST_LOWERCASE; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    for (i=FIRST_UPPERCASE; i<=LAST_UPPERCASE; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    DFA->rowToTokenType[currentRow] = START_STATE;
    
    ++currentRow;
    
    // init identifier row (row 2)
    // NOTE: the 'all others' are already init by calloc (to DEAD_STATE)
    for (i=FIRST_NUMBER; i<=LAST_NUMBER; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    for (i=FIRST_LOWERCASE; i<=LAST_LOWERCASE; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    for (i=FIRST_UPPERCASE; i<=LAST_UPPERCASE; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    DFA->rowToTokenType[currentRow] = IDENT_STATE;
    
    ++currentRow;
    
    // init number row (row 3)
    // NOTE: all chars except digits are already init by calloc (to DEAD_STATE)
    for (i=FIRST_NUMBER; i<=LAST_NUMBER; i++)
        DFA->matrix[currentRow][i] = NUMBER_STATE;
    
    DFA->rowToTokenType[currentRow] = NUMBER_STATE;
    
    ++currentRow;
    
    // build DFA matrix rows for (partial/full) reserved words
    ignoreCharacter = false; // first word only, default to false
    for (int i=0; i<words_len; i++)
    {
        for (int j=0;  ; j++)
        {
            if ( words[i][j] == '\0')
                break;
            
            if ( (ignoreCharacter) && (words[i-1][j] != words[i][j]) )
            {
                ignoreCharacter = false;
                // link coresp col in proper prior row to this row
                if ( j > 0 )
                {
                    int k=i-1;
                    int l=currentRow;
                    while ( (k>0) && (words[k][j-1] == words[i][j-1]) )
                    {
                        int m=j;
                        while ( words[k][m] != '\0' )
                        {
                            --l;
                            ++m;
                        }
                        --k;
                    }
                    --l;
                    DFA->matrix[l][words[i][j]] = currentRow;
                }
            }
            
            if ( !ignoreCharacter )
            {
                // if first letter of word, point corresp col in start row at this row
                if ( j == 0 )
                    DFA->matrix[1][words[i][j]] = currentRow;
                
                // init row... all others inputs init by calloc to dead state
                for (int k=FIRST_NUMBER;    k<=LAST_NUMBER;    k++)
                    DFA->matrix[currentRow][k] = IDENT_STATE; // (non-leading numbers) ident
                
                for (int k=FIRST_LOWERCASE; k<=LAST_LOWERCASE; k++)
                    DFA->matrix[currentRow][k] = IDENT_STATE; // (cap letters) ident
                
                for (int k=FIRST_UPPERCASE; k<=LAST_UPPERCASE; k++)
                    DFA->matrix[currentRow][k] = IDENT_STATE; // (low letters) ident
                
                // if there is another char in this keyword, point corresp column to next row
                if ( words[i][j+1] != '\0')
                    DFA->matrix[currentRow][words[i][j+1]] = currentRow+1;
                
                // if there is another char in this keyword, this row is ident
                if ( words[i][j+1] != '\0')
                    DFA->rowToTokenType[currentRow] = IDENT_STATE;
                else
                    DFA->rowToTokenType[currentRow] = words_token[i]; // TODO: should map to reserved word's token number
                
                ++currentRow;
            }
        }
        ignoreCharacter = true; // default true all words but first
    }
    
    
    // build DFA matrix rows for (single/compound) symbols
    ignoreCharacter = false; // first symbol (possibly compound), default to false
    for (int i=0; i<symbols_len; i++)
    {
        for (int j=0;  ; j++)
        {
            if ( symbols[i][j] == '\0')
                break;
            
            if ( (ignoreCharacter) && (symbols[i-1][j] != symbols[i][j]) )
            {
                ignoreCharacter = false;
                // link coresp col in proper prior row to this row
                if ( j > 0 )
                {
                    int k=i-1;
                    int l=currentRow;
                    while ( (k>0) && (symbols[k][j-1] == symbols[i][j-1]) )
                    {
                        int m=j;
                        while ( symbols[k][m] != '\0' )
                        {
                            --l;
                            ++m;
                        }
                        --k;
                    }
                    --l;
                    DFA->matrix[l][symbols[i][j]] = currentRow;
                }
            }
            
            if ( !ignoreCharacter )
            {
                // if first char of symbol, point corresp col in start row at this row
                if ( j == 0 )
                    DFA->matrix[1][symbols[i][j]] = currentRow;
                
                // if there is another char in this symbol, point corresp column to next row
                if ( symbols[i][j+1] != '\0' )
                    DFA->matrix[currentRow][symbols[i][j+1]] = currentRow+1;
                
                // if there is another char in this symb, this row is dead state
                if ( symbols[i][j+1] != '\0' )
                    DFA->rowToTokenType[currentRow] = 0;
                else
                    DFA->rowToTokenType[currentRow] = symbols_token[i];
                
                ++currentRow;
            }
        }
        ignoreCharacter = true; // default true all words but first
    }
    
    
    return DFA;
}


// displays errors.. duh!
void displayError(int code, int var)
{
    const int maxErrMsgLength = 200;
    
    char* errMsg = malloc(sizeof(char)*(maxErrMsgLength+1));
    memset(errMsg, '\0', maxErrMsgLength+1);

    switch(code)
    {
        case(1):
            
            strcpy(errMsg, "DFA: finished in dead state\n");
            break;
            
        case(3):
            
            // this error message required by assignment specs
            strcpy(errMsg, "Identifier too long\n");
            break;
            
        case(4):
            
            // this error message required by assignment specs
            strcpy(errMsg, "Number out of acceptable range\n");
            break;

            
        case(10):
            
            // this error message required by assignment specs
            strcpy(errMsg, "Identifier cannot start with a number\n");
            break;
            
        case(11):
            
            // this error message required by assignment specs
            strcpy(errMsg, "Invalid token\n");
            break;
            
        case(12):
            strcpy(errMsg, "displaySourceFile invalid file pointer\n");
            break;
            
            
        case(13):
            strcpy(errMsg, "removeComments invalid input file pointer\n");
            break;
            
        case(14):
            
            strcpy(errMsg, "removeComments invalid clean output file pointer\n");
            break;
            
        case(15):
            
            strcpy(errMsg, "Too many arguments. Allowed: (String) filename, \"--source\", \"--clean\"\n");
            break;
            
        case(16):
            
            strcpy(errMsg, "Error: invalid arg. Allowed: (String) filename, \"--source\", \"--clean\"\n");
            break;
            
        default:
            
            strcpy(errMsg, "unknown error occured\n");
            break;
    }
    
    printf("%s\n", errMsg);
    return;
} // END display error


// Pre-Conditions: Takes in a valid index file pointer.
// Post-Conditions: Prints the contents of the file to the user.
void displaySourceFile(FILE* ifp){
    
    char c;
    
    //Check to make sure File Pointer is not NULL.
    //If NULL then return from function.
    if (ifp == NULL)
    {
        displayError(12, 0);
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


void  displayToken(aToken_type* t, const char* tokenNames[])
{
    
    if(t->t == 2)
    {
        printf("%-13s", t->val.identifier);
    }
    else if(t->t == 3)
    {
        printf("%-13d", t->val.number);
    }
    else
    {
        printf("%-13s", tokenNames[t->t]);
    }
    
    printf("%d\n", t->t);
    
    return;
    
} // END display token


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


//  returns next token starting at current position of file pointer
aToken_type* getNextToken(FILE* cleanFile, DFA_type* DFA)
{
    
    bool moveFilePointerOneBack = true;
    
    int length         = 0;
    int DFAstate       = START_STATE;
    int DFAstate_prev  = DEAD_STATE;
    int i              = 0;
    
    // longest identifier is 12 but a number with many leading 0's could be valid
    // ! things could go BAD if a number longer than 254 digits long is read !
    char* lexeme = calloc(255, sizeof(char));
    char c = 9; // default to tab
    
    aToken_type* t = (aToken_type*)malloc(sizeof(aToken_type));
    
    // return nullsym token on eof
    if (feof(cleanFile))
    {
        t->t = 1;
        return t;
    }
    
    while(true)
    {
        moveFilePointerOneBack = true;
        
        fscanf(cleanFile, "%c", &c);
        
        if( feof(cleanFile) )
        {
            DFAstate = DEAD_STATE;
            moveFilePointerOneBack = false;
            break;
        }
        
        /* loop (if on first char of lexeme) until a non-ignored character is read in (break for EOF) */
        while( (length == 0) && isIgnoredChar(c) && !feof(cleanFile) )
        {
            fscanf(cleanFile, "%c", &c);
        }
        
        // handle eof-only lexeme
        if( feof(cleanFile) && (length == 0) && isIgnoredChar(c) )
        {
            DFAstate = DEAD_STATE;
            moveFilePointerOneBack = false;
            break;
        }
        
        /* handle ignored characters following non-null lexeme */
        if( (length > 0) && isIgnoredChar(c) )
        {
            moveFilePointerOneBack = false;
            break;
        }
        
        // stop/error on invalid column number ... BUT NOT EOF!!!
        if( c > DFA_MATRIX_NUMBER_COLUMNS )
        {
            DFAstate = DEAD_STATE; // signal halt to caller
            displayError(2,c);
            break;
        }
        
        // get next DFA state
        DFAstate_prev = DFAstate;
        DFAstate = DFA->matrix[DFAstate][c];
        
        // check if (current) state is dead, previously was a number, and input was letter
        if( (DFAstate == DEAD_STATE) && (DFAstate_prev == NUMBER_STATE) && isLetter(c) )
        {
            DFAstate = DEAD_STATE; // signal halt to caller
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
        if( (length >= MAX_IDENTIFIER_LENGTH) && (DFAstate == IDENT_STATE) )
        {
            DFAstate = DEAD_STATE; // signal halt to caller
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
    
    //  convert state to token type and store
    t->t = DFA->rowToTokenType[DFAstate];
    
    switch(DFA->rowToTokenType[DFAstate])
    {
        // halt (w/o error msg)
        case(DEAD_STATE):
            
            t->t = 1; // signal halt to caller
            free(lexeme);
            break;
            
        // invalid token type, i.e. invalid symbol or operator
        case(START_STATE):
            
            displayError(11, 0);
            t->t = 1; // signal halt to caller
            free(lexeme);
            break;
            
        // identifier token -- lexeme must live
        case(IDENT_STATE):
            
            t->val.identifier = lexeme;
            break;
            
        // number token
        case(NUMBER_STATE):
            
            sscanf(lexeme, "%d", &i);
            if ( (i > 32767) || (i < -32768) )
            {
                // display error and signal halt
                t->t          = 1;
                DFAstate      = DEAD_STATE;
                displayError(4, i);
                break;
            }
            else
            {
                t->val.number = i;
            }
            
            free(lexeme);
            break;
            
        // symbol token (no other values need be stored)
        default:
            
            free(lexeme);
            break;
    }
    
    return t;
    
} // END get next token


// returns true if character should be ignored by lexer
bool isIgnoredChar(int c)
{
    const int tab=9, cr=13, lf=10, space=32;
    
    if ((c == tab) || (c == cr) || (c == lf) || (c == space))
        return true;
    
    return false;
}


// returns true if character is a letter
bool isLetter(int c)
{
    const int FIRST_LOWERCASE = 97;
    const int  LAST_LOWERCASE = 122;
    const int FIRST_UPPERCASE = 65;
    const int  LAST_UPPERCASE = 90;
    
    if ( ((c >= FIRST_UPPERCASE) && (c <= LAST_UPPERCASE)) ||
         ((c >= FIRST_LOWERCASE) && (c <= LAST_LOWERCASE))    )
        return true;
    
    return false;
    
}

//tiny helper function
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
int setOptions(int argc, char* argv[], struct Options* optns){
    
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
        if(strcmp(argv[i], "--source") == 0){
            optns->show_source = true;
        }
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


