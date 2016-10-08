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

#define MAX_IDENTIFIER_LENGTH    12
#define MAX_ASCII_INPUT         126

#define DEAD_STATE                0
#define START_STATE               1
#define IDENT_STATE               2
#define NUMBER_STATE              3
#define NULL_TOKEN                1

// ascii code boundaries
#define FIRST_NUMBER             48
#define LAST_NUMBER              57
#define FIRST_LOWERCASE          97
#define LAST_LOWERCASE          122
#define FIRST_UPPERCASE          65
#define LAST_UPPERCASE           90

union token_value
{
    int   number;
    char* identifier;
};

typedef struct
{
    union token_value val;
    size_t type;
    
} aToken_type;

struct Options {
    bool show_source;
    bool show_clean;
    char filename[255];
};

typedef struct DFA {
    char** tokenNames;
    int** matrix;
    int* matrixRowToTokenType;
    int numMatrixRows;
} DFA_type;

// function prototypes

aToken_type* getNextToken(FILE* cleanFile, DFA_type* DFA);

bool isIgnoredChar(int c);
bool isLetter(int c);
bool isNumber(int c);

DFA_type* DFAmaker(char** words, size_t words_len, int* words_token,
                   char** symbols, size_t symbols_len, int* symbols_token);

int  findFirstDifference(char* a, char* b);
int  lengthToStringEnd(char* a, size_t p);
int  setOptions (int argc, char* argv[], struct Options* Options);
int  removeComments (FILE* infile, FILE* cleanFile);

void bubbleConSort(char** list, int* indices, size_t len, int i);
void displayError(int code, int var);
void displaySourceFile(FILE* ifp);
void displayToken(aToken_type* t, const char* tokenNames[]);
void freeToken(aToken_type* t);
void readnextc(FILE* f, char* buff);


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
    char* words[]     = {"const","var","procedure","call","begin","end","else","if","then","while","do","read","write","odd"};
    int words_token[] = {     28,   29,         30,    27,     21,   22,    33,  23,    24,    25,   26,    32,     31,    8};
    size_t words_len = sizeof(words) / sizeof(char *);
    
    char* syms[]     = {"+","-","*","/","=","<>","<=","<",">=",">",":=",",",";",".","(",")"};
    int syms_token[] = {  4,  5,  6,  7,  9,  10,  12, 11,  14, 13,  20, 17, 18, 19, 15, 16};
    size_t syms_len = sizeof(syms) / sizeof(char *);
    
    //  token names are ordered by their (sequential) token type ordinal values
    const char* tokenNames[] = {
        "PLACE_HOLDER", "nulsym", "identsym", "numbersym", "+", "-", "*", "/",
        "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=",
        "begin", "end", "if", "then", "while", "do", "call", "const", "var",
        "procedure", "write", "read", "else"};
    
    DFA_type* DFA = DFAmaker(words, words_len, words_token, syms, syms_len, syms_token);
    
    printf("\n");
    printf("tokens:\n");
    printf("-------\n");
    
    /* lex */
    while(true)
    {
        aToken_type* toke = getNextToken(cleanFile, DFA);
        
        // halt if nullsym is returned
        if ( toke->type == 1 )
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
    
    int i=0, j=0, currentRow=0;
    
    // intially we have dead, start, ident and number states/rows
    int DFAmatrixRows=4;
    
    DFA_type* DFA = malloc(sizeof(DFA_type));
    
    // sort word and symbol lists alphabetically
    bubbleConSort(words, words_token, words_len, 1);
    bubbleConSort(symbols, symbols_token, symbols_len, 1);
    
    // calculate number of DFA rows to accomodate (partial/complete) reserved word states
    i=0;
    ignoreCharacter = false;
    
    while ( i<words_len )
    {
        j=0;
        
        while ( words[i][j] != '\0' )
        {
            // stays true for remainder of a word after first trip
            if ( (i>0) && (words[i-1][j] !=  words[i][j]))
                ignoreCharacter = false;
            
            if ( !ignoreCharacter )
                ++DFAmatrixRows;
            
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
                ++DFAmatrixRows;
            
            ++j;
        }
        i++;
        ignoreCharacter = true;
    }
    
    DFA->numMatrixRows = DFAmatrixRows;
    
    // this is for translate final matrix row -> token type
    DFA->matrixRowToTokenType = calloc(DFAmatrixRows, sizeof(int));
    
    DFA->matrix = malloc(sizeof(int*)*DFAmatrixRows);
    for (i=0; i<DFAmatrixRows; i++)
        DFA->matrix[i] = calloc(MAX_ASCII_INPUT+1,sizeof(int)); // chars 0-126 are inputs/columns
    
    
    /* INITIALIZE MATRIX */
    
    
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
    
    DFA->matrixRowToTokenType[currentRow] = START_STATE;
    
    ++currentRow;
    
    // init identifier row (row 2)
    // NOTE: the 'all others' are already init by calloc (to DEAD_STATE)
    for (i=FIRST_NUMBER; i<=LAST_NUMBER; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    for (i=FIRST_LOWERCASE; i<=LAST_LOWERCASE; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    for (i=FIRST_UPPERCASE; i<=LAST_UPPERCASE; i++)
        DFA->matrix[currentRow][i] = IDENT_STATE;
    
    DFA->matrixRowToTokenType[currentRow] = IDENT_STATE;
    
    ++currentRow;
    
    // init number row (row 3)
    // NOTE: all chars except digits are already init by calloc (to DEAD_STATE)
    for (i=FIRST_NUMBER; i<=LAST_NUMBER; i++)
        DFA->matrix[currentRow][i] = NUMBER_STATE;
    
    DFA->matrixRowToTokenType[currentRow] = NUMBER_STATE;
    
    ++currentRow;
    
    
    /* BUILD MATRIX ROWS FOR (PARTIAL/FULL) RESERVED WORDS */
    
    ignoreCharacter = false;
    
    // i (ordinal)  controls which which word we are on
    for (int i=0; i<words_len; i++)
    {
        if ( i==6 )
            printf("");
        
        // j (ordinal) controls which character of word we are on
        for (int j=0;  ; j++)
        {
            
            if ( words[i][j] == '\0')
                break;
            
            // check if substring we've read is unique and requires a row/state be dedicated to it
            if ( (ignoreCharacter) && (words[i-1][j] != words[i][j]) )
            {
                ignoreCharacter = false;
                
                // if this isn't the fist char of word, we need to search for proper (prior) row
                // to add a link/state change on this input char to point tothis current state/row
                if ( j > 0 )
                {
                    int k=i-1;
                    int l=currentRow-1;
                    // subtract number of matrix rows generated by characters in j through (end) position of word[k]
                    l=l-lengthToStringEnd(words[k], j);
                    int m=255;
                    
                    // keep looking back 'til we find a pair that differ in same or earlier place than j and j-1
                    while ( (m>j) && (k>0) )
                    {
                        m=findFirstDifference(words[k], words[k-1]);
                        if (m<j)
                            break;
                        l=l-lengthToStringEnd(words[k-1], m);
                        k--;
                    }
                
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
                    DFA->matrixRowToTokenType[currentRow] = IDENT_STATE;
                else
                    DFA->matrixRowToTokenType[currentRow] = words_token[i]; // TODO: should map to reserved word's token number
                
                ++currentRow;
            }
        }
        ignoreCharacter = true; // default true all words but first
    }
    
    
    /* BUILD MATRIX ROWS FOR (PARTIAL/FULL) SYMBOLS/OPERATORS */
    
    ignoreCharacter = false;
    
    // i ordinal controls which symbol/operator we are on
    for (int i=0; i<symbols_len; i++)
    {
        // j ordinal controls which character of symbol/operator we are considering
        for (int j=0;  ; j++)
        {
            // stop j itterations at symbol/operator termination
            if ( symbols[i][j] == '\0')
                break;
            
            // check if substring we've read is unique and requires a row/state be dedicated to it
            if ( (ignoreCharacter) && (symbols[i-1][j] != symbols[i][j]) )
            {
                ignoreCharacter = false;
                
                // if this isn't the fist char of symbol, we need to search for proper (prior) row
                // to add a link/state change on this input char to point tothis current state/row
                if ( j > 0 )
                {
                    int k=i-1;
                    int l=currentRow-1;
                    // subtract number of matrix rows generated by characters in j through (end) position of symbols[k]
                    l=l-lengthToStringEnd(symbols[k], j);
                    int m=255;
                    
                    // keep looking back 'til we find a pair that differ in same or earlier place than j and j-1
                    while ( (m>j) && (k>0) )
                    {
                        m=findFirstDifference(symbols[k], symbols[k-1]);
                        if (m<j)
                            break;
                        l=l-lengthToStringEnd(symbols[k-1], m);
                        k--;
                    }
                    
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
                    DFA->matrixRowToTokenType[currentRow] = 0;
                else
                    DFA->matrixRowToTokenType[currentRow] = symbols_token[i];
                
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


// displays token information
void  displayToken(aToken_type* t, const char* tokenNames[])
{
    
    if(t->type == 2)
    {
        printf("%-13s", t->val.identifier);
    }
    else if(t->type == 3)
    {
        printf("%-13d", t->val.number);
    }
    else
    {
        printf("%-13s", tokenNames[t->type]);
    }
    
    printf("%zu\n", t->type);
    
    return;
    
} // END display token


// frees all memory used by an aToken_type
void  freeToken(aToken_type* t)
{
    
    if(t != NULL)
    {
        if(t->type== 2)
        {
            free(t->val.identifier);
        }
        
        free(t);
    }
    
    return;
    
} // END free token


// returns ordinal of first position in which strings differ, -1 if identical
// ASSUMES STRINGS PROPERLY TERMINATED
int findFirstDifference(char* a, char* b)
{
    for (int i=0; ; i++)
    {
        if ( (a[i]=='\0') && (b[i]=='\0') )
            return -1;
        
        if ( a[i] != b[i] )
            return i;
    }
}

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
    
    while( true )
    {
        moveFilePointerOneBack = true;
    
        fscanf(cleanFile, "%c", &c);
        
        if ( feof(cleanFile) )
        {
            if  ( length == 0 )
                DFAstate = DEAD_STATE;
            
            moveFilePointerOneBack = false;
            break;
        }
        
        /* loop (if on first char of lexeme) until a non-ignored character is read in (break for EOF) */
        while( (length == 0) && isIgnoredChar(c) && !feof(cleanFile) )
        {
            fscanf(cleanFile, "%c", &c);
        }
        
        if( isIgnoredChar(c) )
        {
            // eof and all token are created, so halt
            if ( length == 0)
                DFAstate = DEAD_STATE;
            
            moveFilePointerOneBack = false;
            break;
        }
        
        // halt on character out of range
        if( c > MAX_ASCII_INPUT )
        {
            DFAstate = DEAD_STATE;
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
        
        // check if (current) state is dead, previous was a symbol/operator, and input was non-num/letter
        if ( (DFAstate == DEAD_STATE) && (DFAstate_prev >3) && !isLetter(c) && !isNumber(c) )
        {
            DFAstate = START_STATE;
            break;
        }
        
        // check if (current) state is dead
        if( DFAstate == DEAD_STATE )
        {
            // roll back state
            DFAstate = DFAstate_prev;
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
    t->type= DFA->matrixRowToTokenType[DFAstate];
    
    // make approp token based on final state
    switch(DFA->matrixRowToTokenType[DFAstate])
    {
    case(DEAD_STATE):
        
        t->type = NULL_TOKEN;
        free(lexeme);
        break;
        
    // invalid token type
    case(START_STATE):
        
        displayError(11, 0);
        t->type = NULL_TOKEN;
        free(lexeme);
        break;
        
    // identifier token -- lexeme must live
    case(IDENT_STATE):
        
        // identifier that's too long
        if ( length >= MAX_IDENTIFIER_LENGTH )
        {
            DFAstate = DEAD_STATE; // signal halt to caller
            displayError(3, 0);
        }
        else
        {
            t->val.identifier = lexeme;
        }
            
        break;
        
    // number token -- check if number in range
    case(NUMBER_STATE):
        
        sscanf(lexeme, "%d", &i);
        if ( (i > 32767) || (i < -32768) )
        {
            // display error and signal halt
            t->type         = 1;
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
        
    // symbol or operator token -- no additional data needs to be stored
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
    if ( ((c >= FIRST_UPPERCASE) && (c <= LAST_UPPERCASE)) ||
        ((c >= FIRST_LOWERCASE) && (c <= LAST_LOWERCASE))    )
        return true;
    
    return false;
}

// returns true if character is a digit
bool isNumber(int c)
{
    if ( (c>=FIRST_NUMBER) && (c<=LAST_NUMBER) )
        return 1;
    
    return 0;
}


// returns number of chars in string from position p to end
// ASSUMES STRING IS PROPERLY TERMINATED AT OR AFTER p
int lengthToStringEnd(char* a, size_t p)
{
    int i=-1;
    
    do
    {
        i++;
    } while (a[p+i] != '\0');
    
    return i;
}

// tiny helper function used in removeComments
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


