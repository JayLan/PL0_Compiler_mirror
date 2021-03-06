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
//|         DFAmaker generates a DFA next-state matrix and
//|         a state/row-to-token-type list from
//|
//|===========================================================================

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"


/*                     */
/* DEFINES AND STRUCTS */
/*                     */

#define MAX_IDENTIFIER_LENGTH    12
#define MAX_ASCII_INPUT         126

#define DEAD_STATE                0
#define START_STATE               1
#define IDENT_STATE               2
#define NUMBER_STATE              3
#define NULL_TOKEN                1

/* ascii code boundaries */
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

struct Options
{
    bool show_source;
    bool show_clean;
    char filename[255];
};

typedef struct DFA
{
    char**   tokenNames;
    size_t** nextStateMatrix;
    size_t*  stateToTokenTypeList;
    size_t   rows;
    size_t   numFirstSymbolRowInMatrix;
} DFA_type;


/*                     */
/* FUNCTION PROTOTYPES */
/*                     */


void         bubbleConSort(char** list, size_t* indices, size_t len, size_t i);
size_t       DFA_calcNumberOfRows(char** words,   size_t words_len,
                                  char** symbols, size_t symbols_len);
DFA_type*    DFA_maker(char** words,   size_t words_len,   size_t* words_token,
                       char** symbols, size_t symbols_len, size_t* symbols_token);
void         DFA_matrixRowInit(size_t** matrix, size_t rowNumber, size_t initStateForNumbers,
                              size_t initStateForUppercase, size_t initStateForLowercase );
void         displayError(size_t code, int var);
void         displaySourceFile(FILE* ifp);
void         displayToken(aToken_type* t, const char* tokenNames[]);
int          findFirstDifference(char* a, char* b);
void         freeToken(aToken_type* t);
aToken_type* getNextToken(FILE* cleanFile, DFA_type* DFA);
bool         isIgnoredChar(char c);
bool         isLetter(char c);
bool         isNumber(char c);
size_t       lengthToStringEnd(char* a, size_t p);
void         readNextChar(FILE* f, char* buff);
int          setOptions (int argc, char* argv[], struct Options* Options);
int          removeComments (FILE* infile, FILE* cleanFile);


/*           */
/* FUNCTIONS */
/*           */


int main(int argc, char* argv[])
{
    
    /*                */
    /* VARIABLES, ETC */
    /*                */
    
    
    struct Options* optns = malloc(sizeof(struct Options));
    setOptions(argc, argv, optns);
    
    FILE* rawFile   = fopen(optns->filename, "rb+");
    FILE* cleanFile = fopen("clean.pl0",     "wb+");
    
    /* reserved words and token values... can be given in any order (as long */
    /* as words/token values are specified in the same relative ordering     */
    char*  words[]       = {"const","var","procedure","call","begin","end","else","if","then","while","do","read","write","odd"};
    size_t words_token[] = {     28,   29,         30,    27,     21,   22,    33,  23,    24,    25,   26,    32,     31,    8};
    size_t words_len = sizeof(words) / sizeof(char *);
    
    /* symbols/operatorsand token values... can be given in any order (as long */
    /* as symbols/token values are specified in the same relative ordering     */
    char*  syms[]       = {"+","-","*","/","=","<>","<=","<",">=",">",":=",",",";",".","(",")"};
    size_t syms_token[] = {  4,  5,  6,  7,  9,  10,  12, 11,  14, 13,  20, 17, 18, 19, 15, 16};
    size_t syms_len = sizeof(syms) / sizeof(char *);
    
    /* token names - must be ordered by their (sequential) token-type values (0,1,2,...)   */
    /* this list could be generated from the arrays of words, symbols and their token      */
    /* values above BUT that would be restricting, e.g. we couldn't have plussym displayed */
    /* as plussym, only the actual operator "+" it represents                              */
    const char* tokenNames[] = {
        "PLACE_HOLDER", "nulsym", "identsym", "numbersym", "+", "-", "*", "/",
        "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=",
        "begin", "end", "if", "then", "while", "do", "call", "const", "var",
        "procedure", "write", "read", "else"};
    
    
    /*           */
    /* EXECUTION */
    /*           */
    
    
    /*  display original code */
    if( optns->show_source == true )
    {
        printf("\n");
        printf("source code:\n");
        printf("------------\n");
        displaySourceFile(rawFile);
    }
    
    /* produce comment-free code */
    removeComments(rawFile, cleanFile);
    
    fclose(rawFile);
    fclose(cleanFile);
    
    cleanFile = fopen("clean.pl0", "rb+");
    
    /* display comment-free code */
    if( optns->show_clean == true )
    {
        printf("\n");
        printf("source code without comments:\n");
        printf("-----------------------------\n");
        displaySourceFile(cleanFile);
    }
    
    /* setup DFA */
    DFA_type* DFA = DFA_maker(words, words_len, words_token, syms, syms_len, syms_token);
    
    /* make and output tokens */
    printf("\n");
    printf("tokens:\n");
    printf("-------\n");
    
    while( true )
    {
        aToken_type* toke = getNextToken(cleanFile, DFA);
        
        // halt if nullsym is returned
        if( toke->type == NULL_TOKEN )
            break;
        
        displayToken(toke, tokenNames);
        freeToken(toke);
    }
    
    /* clean up */
    fclose(cleanFile);
    return 0;
    
} // END main


/* bubble sorts one array based on its values (strcmp) and simultaneously */
/* reorders other array using same pattern                                */
void bubbleConSort(char** list, size_t* indices, size_t len, size_t x)
{
    if( (list == NULL) || (indices == NULL) )
        return;
    
    if( (x<1) || (x>2) )
    {
        printf("invalid index of which array to sort by (1 or 2 only)\n");
        return;
    }
    
    for( size_t i=0; i<len; i++ )
    {
        for( size_t j=i+1; j<len; j++ )
        {
            if( ((x==1) && (strcmp(list[i],list[j]) > 0)) ||
               ((x==2) && (indices[i] > indices[j]))        )
            {
                char* t = list[i];
                list[i] = list[j];
                list[j] = t;
                
                size_t s   = indices[i];
                indices[i] = indices[j];
                indices[j] = s;
            }
        }
    }
    return;
    
} // END bubbleConSort


size_t DFA_calcNumberOfRows(char** words,   size_t words_len,
                            char** symbols, size_t symbols_len)
{
    /*                                                                     */
    /* CALCULATE NUMBER OF ROWS TO ACCOMODATE RESERVED-WORD RELATED STATES */
    /*                                                                     */
    
    size_t i=0,j=0;
    
    /* intially we have: dead, start, ident and number states/rows */
    size_t numberOfRows = 4;
    
    /* for the first word on list, start counting new rows/states  */
    /* from the very first letter (there's nothing nothing earlier */
    /* in the list to compare it to anyway)                        */
    bool ignoreCharacter = false;
    
    /* i (ordinal) controls which word on list we are considering  */
    i=0;
    while( i<words_len )
    {
        /* j (ordinal) denotes which character of word[i] we are considering */
        j=0;
        while( words[i][j] != '\0' )
        {
            /* only need to stop ignoring chars in word[i], and start counting  */
            /* new rows/states corresponding to initial strings of word[i] when */
            /* we find an initial string (chars 0-j) we haven't encountered     */
            /* before... once we start, we must make new rows/states based on   */
            /* each of the characters reamining in the word                     */
            if( (ignoreCharacter == true) && (i>0) && (words[i-1][j] !=  words[i][j]))
                ignoreCharacter = false;
            
            if( !ignoreCharacter )
                ++numberOfRows;
            
            ++j;
        }
        
        ++i;
        
        /* default to ignore characters (not counting new rows/states) for */
        /* every word except for the first word on the list                */
        ignoreCharacter = true;
    }
    
    
    /*                                                                     */
    /* CALCULATE NUMBER OF ROWS/STATES TO ACCOMODATE SYMBOL-RELATED STATES */
    /*                                                                     */
    
    
    /* for the first symbol/operator on list, start counting new */
    /* rows/states from the very first letter (there's nothing   */
    /* nothing earlier in the list to compare it to anyway)      */
    ignoreCharacter = false;
    
    /* i (ordinal) controls which symbol/operator on list we are considering */
    i=0;
    while( i<symbols_len )
    {
        /* j (ordinal) denotes which char of symbols[i] we are considering */
        j=0;
        while( symbols[i][j] != '\0' )
        {
            /* only need to stop ignoring chars in word[i], and start counting */
            /* new rows/states corresponding to initial strings of symbols[i]  */
            /* when find an initial string (chars 0-j) we haven't encountered  */
            /* before... once we start, we must make new rows/states for each  */
            /* of the characters reamining in the word                         */
            if( (i>0) && (symbols[i-1][j] !=  symbols[i][j]) )
                ignoreCharacter = false;
            
            if( !ignoreCharacter )
                ++numberOfRows;
            
            ++j;
        }
        
        i++;
        
        /* default to ignore characters (not counting new rows/states) for */
        /* every word except for the first word on the list                */
        ignoreCharacter = true;
    }

    return numberOfRows;
}

/* creates DFA matrix and state-to-token-type table from arrays of words, */
/* symbols and corresponding token numbers... the DFA matrix has colmns   */
/* corresponding to inputs for ASCII chars 0-126 (MAX_ASCII_INPUT) and    */
/* rows corresponding to dead, start, identifiers, numbers, and all the   */
/* partial and full strings of the reserved words and symbols/operators   */
/*                                                                        */
/* ASSUMES ALL POINTERS PASSED ARE NON-NULL, *_len CONTAINS NUMBER OF     */
/* ENTRIES IN RESPECTIVE ARRAY, AND *_token IS IN THE SAME ORDER AS ITS   */
/* RESPECTIVE WORDS/SYMBOLS ARRAY                                         */
DFA_type* DFA_maker(char** words,   size_t words_len,   size_t* words_token,
                    char** symbols, size_t symbols_len, size_t* symbols_token)
{
    
    /*                */
    /* VARIABLES, ETC */
    /*                */
    
    
    size_t i=0;
    
    /* for a character in a word/operator being examined at a given */
    /* moment, do we need to add a new row/state to the DFA matrix  */
    bool ignoreCharacter = false;
    
    /* tracks which row of DFA matrix (and rows-to-token-type list) */
    /* we are constructing at the moment                            */
    size_t currentRow=0;
    
    DFA_type* DFA = malloc(sizeof(DFA_type));
    
    /* sort word and symbol lists by ASCII values and reorder */
    /* their correspoding lists of token-type values to match */
    bubbleConSort(words, words_token, words_len, 1);
    bubbleConSort(symbols, symbols_token, symbols_len, 1);
    
    /* calculate number of rows needed in DFA matrix */
    DFA->rows = DFA_calcNumberOfRows(words, words_len, symbols, symbols_len);
    
    
    /*                                                         */
    /* ALLOCATE AND INIT DFA MATRIX AND ROW-TO-TOKEN-TYPE LIST */
    /*                                                         */
    
    
    /* allocate space for our matrix row/state-to-token-type list */
    DFA->stateToTokenTypeList = calloc(DFA->rows, sizeof(size_t));
    
    /* allocate space for our DFA nextStateMatrix and set all */
    /* state transitions to lead to the dead state row (0)    */
    DFA->nextStateMatrix = malloc(sizeof(size_t*)*DFA->rows);
    
    for( i=0; i<DFA->rows; i++ )
        DFA->nextStateMatrix[i] = calloc(MAX_ASCII_INPUT+1,sizeof(size_t));
    
    /* init row 0 (dead   state) - already done by calloc */
    
    /* init row 1 (start  state) of DFA next-state matrix */
    DFA_matrixRowInit(DFA->nextStateMatrix, 1, NUMBER_STATE, IDENT_STATE, IDENT_STATE);
    
    /* finishing in start state idicates invalid token read */
    DFA->stateToTokenTypeList[1] = NULL_TOKEN;
    
    /* init row 2 (ident  state) of DFA next-state matrix */
    DFA_matrixRowInit(DFA->nextStateMatrix, 2, IDENT_STATE, IDENT_STATE, IDENT_STATE);
    
    DFA->stateToTokenTypeList[2] = IDENT_STATE;
    
    /* init row 3 (number state) of DFA next-state matrix */
    DFA_matrixRowInit(DFA->nextStateMatrix, 3, NUMBER_STATE, DEAD_STATE, DEAD_STATE);
    
    DFA->stateToTokenTypeList[3] = NUMBER_STATE;
    
    
    /*                                                    */
    /* BUILD DFA ROWS FOR ALL PARTIAL/FULL RESERVED WORDS */
    /*                                                    */
    
    
    /* rows/states for keywords (or symbols, if */
    /* there are no words) begin on row 4       */
    currentRow = 4;
    
    /* for the first word on list, start adding new         */
    /* rows/states from the very first letter (there's      */
    /* nothing earlier in the list to compare it to anyway) */
    ignoreCharacter = false;
    
    /* i (ordinal) controls which which word we are considering */
    for( size_t i=0; i<words_len; i++ )
    {
        /* j (ordinal) controls which character of word we are considering */
        for( size_t j=0; ; j++ )
        {
            
            if( words[i][j] == '\0' )
                break;
            
            /* check if substring we've read is first unique one in word[i] */
            /* and requires us to start creating row/states (and to do so   */
            /* for each of the letters remaining in word[i]                 */
            if ( (ignoreCharacter) && (words[i-1][j] != words[i][j]) )
            {
                ignoreCharacter = false;
                
                /* if this isn't the fist char of word, we need to search for */
                /* proper (prior) row to add a next state on this input char  */
                /* words[i][j] to point tothis current state/row              */
                if( j > 0 )
                {
                    /* k controls which prior word's (word[k]) related states we */
                    /* are considering adding a next state entry to for this row */
                    size_t k=i-1;
                    
                    /* l controls which particular row we are considering adding */
                    /* a next-state entry to for this row/state                  */
                    size_t l=currentRow-1;
                    
                    /* decrement l by the number of rows in the matrix preceeding */
                    /* this one dedicated to states generated by the chars in     */
                    /* word[k] at and after the position (j) in which word[i] and */
                    /* word[k] first differ                                       */
                    l -= lengthToStringEnd(words[k], j);
                    
                    /* move back through pairs of adjacent words until we find */
                    /* a pair that differ in the same or an earlier place than */
                    /* than this pair differs (j)                              */
                    size_t m=255;
                    while( (m>j) && (k>0) )
                    {
                        m = findFirstDifference(words[k], words[k-1]);
                        
                        /* if differ in earlier place than j, done */
                        if( m<j )
                            break;
                        
                        /* differ in an equal place, so decrement l by the number */
                        /* of rows in the matrix dedicated to states generated by */
                        /* the chars in word[k] at and after the first position   */
                        /* (m) in which word[k-1] differs from word[k]            */
                        l = l-lengthToStringEnd(words[k-1], m);
                        
                        k--;
                    }
                    
                    /* create next-state for state on row l that points to this row */
                    DFA->nextStateMatrix[l][words[i][j]] = currentRow;
                }
            }
            
            /* if we're looking at characters in word[i] that make for */
            /* initial substrings we haven't encountered before        */
            if( !ignoreCharacter )
            {
                /* if first letter of word, point corresp col in start row here */
                if( j == 0 )
                    DFA->nextStateMatrix[1][words[i][j]] = currentRow;
                
                /*  init current row of DFA next-state matrix */
                DFA_matrixRowInit(DFA->nextStateMatrix, currentRow,
                                  IDENT_STATE, IDENT_STATE, IDENT_STATE);
                
                /* if there is another char in this word, point corresp column */
                /* to the next row                                             */
                if ( words[i][j+1] != '\0' )
                    DFA->nextStateMatrix[currentRow][words[i][j+1]] = currentRow+1;
                
                /* if there is another char in this word, the corresponding   */
                /* row in the row-to-token-type list needs to be set to ident */
                if ( words[i][j+1] != '\0' )
                    DFA->stateToTokenTypeList[currentRow] = IDENT_STATE;
                
                /* if this is last letter, corresponding row in row-to-token-type */
                /* list needs to be set to token-type for reserved word (word[i]) */
                if ( words[i][j+1] == '\0' )
                    DFA->stateToTokenTypeList[currentRow] = words_token[i];
                
                ++currentRow;
            }
        }
        
        /* default true all words after the first */
        ignoreCharacter = true;
    }
    
    
    /*                                                       */
    /* BUILD DFA ROWS FOR ALL PARTIAL/FULL SYMBOLS/OPERATORS */
    /*                                                       */
    
    
    /* for the first word on list, start adding new         */
    /* rows/states from the very first character (there's   */
    /* nothing earlier in the list to compare it to anyway) */
    ignoreCharacter = false;
    
    DFA->numFirstSymbolRowInMatrix = currentRow;
    
    /* i (ordinal) controls which which symbol/operator we are considering */
    ignoreCharacter = false;
    for( size_t i=0; i<symbols_len; i++ )
    {
        /* j (ordinal) controls which character of word we are considering */
        for( size_t j=0;  ; j++)
        {
            
            if( symbols[i][j] == '\0')
                break;
            
            /* check if substring we've read is first unique one in symbols[i] */
            /* and requires us to start creating row/states (and to do so for  */
            /* each of the letters remaining in symbols[i]                     */
            if( ignoreCharacter && (symbols[i-1][j] != symbols[i][j]) )
            {
                ignoreCharacter = false;
                
                /* if this isn't the fist char of symbol, we need to search for */
                /* proper (prior) row to add a next state on this input char    */
                /* words[i][j] to point to this current state/row               */
                if( j > 0 )
                {
                    /* k controls which prior symbol's (symbols[k]) related states  */
                    /* we are considering adding a next-state entry to for this row */
                    size_t k = i-1;
                    
                    /* l controls which particular row we are considering adding */
                    /* a next-state entry to for this row/state                  */
                    size_t l = currentRow-1;
                    
                    /* decrement l by the number of rows in the matrix preceeding */
                    /* this one dedicated to states generated by the chars in     */
                    /* symbols[k] at/after the position (j) in which symbols[i]   */
                    /* and symbols[k] first differ                                */
                    l -= lengthToStringEnd(symbols[k], j);
                    
                    /* move back through pairs of adjacent symbols until we find */
                    /* a pair that differ in the same or an earlier place than   */
                    /* than this pair differs (j)                                */
                    size_t m = j+1;
                    while( (m>j) && (k>0) )
                    {
                        m = findFirstDifference(symbols[k], symbols[k-1]);
                        
                        /* if differ in earlier place than j, done */
                        if ( m<j )
                            break;
                        
                        /* differ in an equal place, so decrement l by the number  */
                        /* of rows in the matrix dedicated to states generated by  */
                        /* the chars in symbols[k] at and after the first position */
                        /* (m) in which symbols[k-1] differs from symbols[k]       */
                        l -= lengthToStringEnd(symbols[k-1], m);
                        
                        k--;
                    }
                    
                    /* create next-state for state on row l that points to this row */
                    DFA->nextStateMatrix[l][symbols[i][j]] = currentRow;
                }
            }
            
            /* if we're looking at characters in symbols[i] that make for */
            /* initial substrings we haven't encountered before           */
            if( !ignoreCharacter )
            {
                /* if first letter of word, point corresp col in start row here */
                if( j == 0 )
                    DFA->nextStateMatrix[1][symbols[i][j]] = currentRow;
                
                /* if there is another char in this symbol, point   */
                /* corresp column on this row to the next row/state */
                if( symbols[i][j+1] != '\0' )
                    DFA->nextStateMatrix[currentRow][symbols[i][j+1]] = currentRow+1;
                
                /* if there is another char in this symbol, the corresponding */
                /* row in the row-to-token-type list needs to be set to dead  */
                if( symbols[i][j+1] != '\0' )
                    DFA->stateToTokenTypeList[currentRow] = 0;
                
                /* if this is last char, corresponding row in the row-to-token-type */
                /* list needs to be set to the token-type for this symbol/operator  */
                if( symbols[i][j+1] == '\0' )
                    DFA->stateToTokenTypeList[currentRow] = symbols_token[i];
                
                ++currentRow;
            }
        }
        
        /* default true all sybols/operators after the first */
        ignoreCharacter = true;
    }
    
    return DFA;
    
} // END DFAmaker


/* initializes a DFA next-state matrix row             */
/* ASSUMES MATRIX NON-NULL AND CALLOC'D DEAD_STATE (0) */
void DFA_matrixRowInit(size_t** matrix, size_t rowNumber, size_t initStateForNumbers,
                       size_t initStateForUppercase, size_t initStateForLowercase  )
{
    
    for( size_t k=FIRST_NUMBER;    k<=LAST_NUMBER;    k++ )
    {
        matrix[rowNumber][k] = initStateForNumbers;
    }
    
    for( size_t k=FIRST_LOWERCASE; k<=LAST_LOWERCASE; k++ )
    {
        matrix[rowNumber][k] = initStateForLowercase;
    }
    
    for( size_t k=FIRST_UPPERCASE; k<=LAST_UPPERCASE; k++ )
    {
        matrix[rowNumber][k] = initStateForUppercase;
    }
    
    return;
    
} // END DFAmatrixRowInit


/* displays errors */
void displayError(size_t code, int var)
{
    const size_t maxErrMsgLength = 200;
    
    char* errMsg = malloc(sizeof(char)*(maxErrMsgLength+1));
    memset(errMsg, '\0', maxErrMsgLength+1);
    
    switch( code )
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
    
} // END displayError


/* Pre-Conditions: Takes in a valid index file pointer.          */
/* Post-Conditions: Prints the contents of the file to the user. */
void displaySourceFile(FILE* ifp){
    
    char c;
    
    /* Check to make sure File Pointer is not NULL */
    if( ifp == NULL )
    {
        displayError(12, 0);
        return;
    }
    
    /* Read in contents until end of file is reached */
    /* and display file content                      */
    while( true )
    {
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
    
    /* movie file pointer back to start of file */
    fseek(ifp, 0, SEEK_SET);
    
    return;
    
} // END displaySourceFile


/* displays token information */
/* ASSUMES t IS NON-NULL      */
void displayToken(aToken_type* t, const char* tokenNames[])
{
    
    if( t->type == 2 )
    {
        printf("%-13s", t->val.identifier);
    }
    else if( t->type == 3 )
    {
        printf("%-13d", t->val.number);
    }
    else
    {
        printf("%-13s", tokenNames[t->type]);
    }
    
    printf("%zu\n", t->type);
    
    return;
    
} // END displayToken


/* returns ordinal of first position in which strings differ, -1 if identical */
/* ASSUMES STRINGS PROPERLY TERMINATED                                        */
int findFirstDifference(char* a, char* b)
{
    for( size_t i=0; ; i++ )
    {
        if( (a[i]=='\0') && (b[i]=='\0') )
            return -1;
        
        if( a[i] != b[i] )
            return (int)i;
    }
    
} // END findFirstDifference


/* frees all memory used by an aToken_type */
void freeToken(aToken_type* t)
{
    
    if( t != NULL )
    {
        if( t->type== 2 )
        {
            free(t->val.identifier);
        }
        
        free(t);
    }
    
    return;
    
} // END freeToken


/* returns next token starting at current position of file pointer */
/* ASSUMES cleanFile IS NON-NULL AND DFA IS PROPERLY CONFIGURED    */
aToken_type* getNextToken(FILE* cleanFile, DFA_type* DFA)
{
    
    /*                */
    /* VARIABLES, ETC */
    /*                */
    
    
    /* we must look one char past a lexeme to know when it is    */
    /* finished... if that "extra" char is not an ignored char   */
    /* then it is the begining of the next lexeme and the file   */
    /* pointer must be moved back 1 char so it isnt skipped when */
    /* we read in next lexeme (when getNextToken is next called) */
    bool moveFilePointerOneBack = true;
    
    size_t length = 0;
    
    /* n is used when converting number read in as   */
    /* array of chars into an integer... could be <0 */
    int n = 0;
    
    /* ending in START_STATE indicates an invalid token was read */
    size_t DFAstate      = START_STATE;
    size_t DFAstate_prev = DEAD_STATE;
    
    /* longest identifier is 12 but a number > 12 digits with   */
    /* leading 0's could be valid length once leading 0's are   */
    /* removed... so we make lexeme array to hold up to 254     */
    /* chars/digits... things may go bad if >254 lexeme is read */
    char* lexeme = calloc(255, sizeof(char));
    
    /* init to an ignored char (tab) */
    char c = 9;
    
    aToken_type* t = (aToken_type*)malloc(sizeof(aToken_type));
    
    
    /*                                           */
    /* READ IN NEXT LEXEME, BASIS FOR NEXT TOKEN */
    /*                                           */
    
    
    while( true )
    {
        /* default to moving pointer back after lexeme read */
        moveFilePointerOneBack = true;
        
        fscanf(cleanFile, "%c", &c);
        
        /* if EOF, and lexeme has no chars, nothing to do */
        /* so set up return of null token to signal done  */
        /* (ending in DEAD_STATE returns null token)      */
        if( feof(cleanFile) )
        {
            if( length == 0 )
            {
                DFAstate = DEAD_STATE;
            }
            moveFilePointerOneBack = false;
            break;
        }
        
        /* loop (if on first char of lexeme) until a */
        /* non-ignored char is read in or EOF found  */
        while( (length == 0) && isIgnoredChar(c) && !feof(cleanFile) )
        {
            fscanf(cleanFile, "%c", &c);
        }
        
        /* did we not find a non-ignored char on this loop iter */
        if( isIgnoredChar(c) )
        {
            /* if no lexeme was created, must be at EOF and done */
            /* so set up return of null token to signal done     */
            if ( length == 0)
                DFAstate = DEAD_STATE;
            
            moveFilePointerOneBack = false;
            break;
        }
        
        /* is ASCII char outside range DFA matrix can handle */
        if( c > MAX_ASCII_INPUT )
        {
            /* display invalid token err, set up return of */
            /* null token to signal halt of execution      */
            displayError(2,c);
            DFAstate = DEAD_STATE;
            break;
        }
        
        /* store current state and get next state */
        DFAstate_prev = DFAstate;
        DFAstate = DFA->nextStateMatrix[DFAstate][c];
        
        /* if current state is dead, previous was a number, and input was */
        /* letter we consider it an identifier that started with a number */
        if( (DFAstate == DEAD_STATE)
           && (DFAstate_prev == NUMBER_STATE) && isLetter(c) )
        {
            /* display error and set up null token return to signal halt */
            displayError(10, 0);
            DFAstate = DEAD_STATE;
            break;
        }
        
        //        /* if current state is dead, previous was a symbol/operator, & input  */
        //        /* was non-num/letter, we have an invalid token (bad symbol/operator) */
        //        if ( (DFAstate == DEAD_STATE)
        //              && (DFAstate_prev >= DFA->numFirstSymbolRowInMatrix)
        //              && !isLetter(c) && !isNumber(c) )
        //        {
        //            DFAstate = START_STATE;
        //            break;
        //        }
        
        if( DFAstate == DEAD_STATE )
        {
            // roll back state
            DFAstate = DFAstate_prev;
            break;
        }
        
        /* if we're still in the loop, char we read needs  */
        /* to be appended onto the lexeme we're working on */
        lexeme[length] = c;
        
        ++length;
    }
    
    /* terminate lexeme we just read in */
    lexeme[length] = '\0';
    
    
    /*                               */
    /* CONFIGURE TOKEN AND RETURN IT */
    /*                               */
    
    
    if( moveFilePointerOneBack )
    {
        fseek(cleanFile, -1, SEEK_CUR);
    }
    
    /*  convert state/row in matrix to token type */
    t->type= DFA->stateToTokenTypeList[DFAstate];
    
    
    /* invalid sybols/operators that are initial substrings of valid ones */
    if ( (t->type == DEAD_STATE) && (length > 0)
         && !isLetter(lexeme[0]) && !isNumber(lexeme[0]))
    {
        t->type = START_STATE;
    }
    
    switch(t->type)
    {
        case(DEAD_STATE):
            
            t->type = NULL_TOKEN;
            free(lexeme);
            break;
            
            /* invalid token type */
        case(START_STATE):
            
            displayError(11, 0);
            t->type = NULL_TOKEN;
            free(lexeme);
            break;
            
            /* identifier token - store lexeme in token */
        case(IDENT_STATE):
            
            /* if identifier is too long, show error   */
            /* and return null token to halt execution */
            if( length > MAX_IDENTIFIER_LENGTH )
            {
                t->type = NULL_TOKEN;
                displayError(3, 0);
            }
            else
            {
                t->val.identifier = lexeme;
            }
            break;
            
            /* number token - convert lexeme array to number and store in token */
        case(NUMBER_STATE):
            
            sscanf(lexeme, "%d", &n);
            
            /* if value out-of-range, display error and */
            /* return null token to halt execution      */
            if( (n > 32767) || (n < -32768) )
            {
                t->type = NULL_TOKEN;
                displayError(4, n);
            }
            else
            {
                t->val.number = n;
            }
            
            free(lexeme);
            break;
            
            /* symbol or operator token - token type is all we need to store */
        default:
            
            free(lexeme);
            break;
    }
    
    return t;
    
} // END getNextToken


/* returns true if char should be ignored by lexer */
bool isIgnoredChar(char c)
{
    const int tab=9, cr=13, lf=10, space=32;
    
    if( (c == tab) || (c == cr) || (c == lf) || (c == space) )
    {
        return true;
    }
    
    return false;
    
} // END IsIgnoredChar


/* returns true if char is a letter */
bool isLetter(char c)
{
    if( ((c >= FIRST_UPPERCASE) && (c <= LAST_UPPERCASE)) ||
       ((c >= FIRST_LOWERCASE) && (c <= LAST_LOWERCASE))   )
    {
        return true;
    }
    
    return false;
    
} // END isLetter


/* returns true if character is a digit */
bool isNumber(char c)
{
    if( (c>=FIRST_NUMBER) && (c<=LAST_NUMBER) )
    {
        return 1;
    }
    
    return 0;
    
} // END isNumber


/* returns number of chars in string from position p to termination */
/* ASSUMES STRING IS PROPERLY TERMINATED AT OR AFTER p              */
size_t lengthToStringEnd(char* a, size_t p)
{
    int i=-1;
    
    do
    {
        i++;
    }
    while( a[p+i] != '\0' );
    
    return (size_t)i;
    
} // END lengthToStringEnd


/* tiny helper function used in removeComments */
/* ASSUMES f AND buff ARE NON-NULL             */
void readNextChar(FILE* f, char* buff)
{
    buff[0] = buff[1];
    char c = fgetc(f);
    
    if( !feof(f) )
    {
        buff[1] = c;
    }
    else
    {
        buff[1] = ' ';
    }
    
    return;
    
} // END readNextChar


/* removeComments:  reads a file and removes comments, preserving  */
/*                  whitespace. Returns 0 on success, 1 on failure */
int removeComments(FILE* inFile, FILE* cleanFile)
{
    const int cr=13, lf=10, space=32;
    
    /* check validity of file pointers */
    if( inFile == NULL )
    {
        displayError(13, 0);
        return 1;
    }
    else if( cleanFile == NULL )
    {
        displayError(14, 0);
        return 1;
    }
    
    /* create a buffer to read two char at a time */
    char readbuff[3];
    readbuff[0] = '\0'; readbuff[1] = '\0'; readbuff[2] = '\0';
    
    /* start reading/copying */
    readNextChar(inFile, readbuff);
    while( !feof(inFile) )
    {
        //read a char
        readNextChar(inFile, readbuff);
        
        /* check for start of comment */
        if( strcmp(readbuff, "/*") == 0)
        {
            /* replace begin-comment marker with spaces */
            putc(space, cleanFile);
            putc(space, cleanFile);
            
            /* loop until end of comment */
            do
            {
                readNextChar(inFile, readbuff);
                
                /* replace cr/lf by lf, all other comment chars by spaces */
                if ( (readbuff[1] == cr) || (readbuff[1] == lf) )
                {
                    fputc(lf, cleanFile);
                }
                else
                {
                    fputc(space, cleanFile);
                }
                
            }
            while( (strcmp(readbuff, "*/") != 0) && (!feof(inFile)) );
            
            /* move past comment close chars */
            readNextChar(inFile, readbuff);
            readNextChar(inFile, readbuff);
        }
        
        /* write char to output file */
        fputc(readbuff[0], cleanFile);
        
    }
    
    return 0;
    
} // END removeComments


/* setOptions: reads arguments from cmd line and sorts them into    */
/*             Options struct.  Returns 0 on success, 1 on failure. */
/*             ASSUMES optns IS NON-NULL                            */
int setOptions(int argc, char* argv[], struct Options* optns){
    
    /* if too many args, display error and reutrn 1 */
    if( argc > 4 )
    {
        displayError(15, argc);
        return 1;
    }
    
    /* start with both flags off */
    optns->show_source = false;
    optns->show_clean = false;
    strcpy(optns->filename, "");
    
    /* read args */
    for( size_t i=1; i<argc; ++i ){
        if( strcmp(argv[i], "--source") == 0 )
        {
            optns->show_source = true;
        }
        else if( strcmp(argv[i], "--clean") == 0 )
        {
            optns->show_clean =  true;;
        }
        /* set filename, but only once */
        else if( strcmp(optns->filename, "") == 0 )
        {
            strcpy(optns->filename, argv[i]);
        }
        else
        {
            displayError(16, 0);
            return true;
        }
    }
    
    return false;
    
} // END setOptions

