//|===========================================================================
//|  Team Name: Compiler Builders 33
//|
//|  Programmers:
//|		Megan Chipman
//|		Jason Lancaster
//|		Victoria Proetsch
//|
//|  Course:	COP3402, Fall 2016
//|  Date:      Sept 22, 2016
//|---------------------------------------------------------------------------
//|  Language:	C
//|  File:		parser.c
//|  Purpose:	Assignment #3, Implement a parser and code generator for PL/0
//|
//|  Notes:
//|
//|===========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
	int kind; // const = 1, var = 2, proc = 3
	char* name; // name up to 11 chars
	int val; // number (ASCII value)
	int level; // L level
	int addr; // M address
} symbol;

void program(aToken_type tok);
aToken_type block(aToken_type tok, int level);
aToken_type const_declaration(aToken_type tok, int level);
aToken_type var_declaration(aToken_type tok, int level);
aToken_type proc_declaration(aToken_type tok, int level);
aToken_type statement(aToken_type tok, int level);
aToken_type condition(aToken_type tok, int level);
aToken_type expression(aToken_type tok, int level);
aToken_type term(aToken_type tok, int level);
aToken_type factor(aToken_type tok, int level);
int relation(aToken_type tok);
aToken_type advance(aToken_type tok);

void put_symbol(int kind, char* name, int num, int level, int modifier);
void update_address(int pos, int modifier);
int find_symbol(char* identstr);
int symbol_kind(int symbol_pos);
int symbol_level(int symbol_pos);
int symbol_address(int symbol_pos);
void emit (int op, int l, int m);
void print_pm0(FILE* outFile);
void print_symboltable();
int find_symbol_kind(char* identstr, int kind);

/*SYMBOL  TABLE and PM0 CODE ARRAY AVAIL TO ALL PARSER FUNCTIONS*/
extern aToken_type* tokArr;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction codeArray [MAX_CODE_LENGTH];

/* Global counters */
static int cx = 0;
static int symctr = 0;
static int varctr = 0;


// *** main() is located in compile.c ***
// *** PARSER ***
aToken_type advance(aToken_type tok){

    tok = nextToken();
    return tok;
}

// comments
void program(aToken_type tok){

    //consume first token
    advance(tok);

	tok = block(tok, 0);

	if(tok.t != periodsym){
		error(9);
	}

    //print return instruction
	emit(SIO, 0, 2);

	printf("No errors, the program is syntactically correct.\n");

}

// Processes code block declarations
aToken_type block(aToken_type tok, int l){

    //save location in symbol table
    int prev_symctr = symctr;

    //generate jump
    int jx = cx;
    emit(JMP, 0, 0);

    tok = const_declaration(tok, l);

    //determine amount of space to allocate
    tok = var_declaration(tok, l);
    int space = 4 + varctr;

    //parse any procedures
    tok = proc_declaration(tok, l);

    //update JMP & emit INC
    codeArray[jx].m = cx;
    emit(INC, 0, space);

    tok = statement(tok, l);

    //rewind to previous location in symbol table for scope control
    symctr = prev_symctr;

	return tok;
}

// Handles declarations of constants
aToken_type const_declaration(aToken_type tok, int l){

    //return if there are no constants
	if(tok.t != constsym){
		return tok;
	}

    //add constants to symbol table while they exist
	do {
        tok = advance(tok);

		if(tok.t != identsym){
			error(4);
		}
        char *id = (tok.val).identifier;
		tok = advance(tok);

		if(tok.t != eqsym){
			error(3);
		}
		tok = advance(tok);

		if(tok.t != numbersym){
			error(2);
		}

		//add constant to symbol table
		put_symbol(1, id, tok.t, l, 0);

		tok = advance(tok);

	}while (tok.t == commasym);

	if(tok.t != semicolonsym){
		error(5);
	}

	tok = advance(tok);

	return tok;

}

// Handles declarations of variables
aToken_type var_declaration(aToken_type tok, int l){

    //instead of local variable numvars, must use a global variable accessible to block:
    //the INC instruction must be generated in block to be in the proper order
    varctr = 0;

    //return if there are no variables
	if(tok.t != varsym){
        return tok;
	}

    //count and add vars to symbol table
	do{
        tok = advance(tok);
        if (tok.t != identsym){
            error(4);
        }

        varctr++;

        //add variable to symbol table
        put_symbol(2, tok.val.identifier, 0, l, (3 + varctr));
        tok = advance(tok);

	}while(tok.t == commasym);

    if(tok.t != semicolonsym){
        error(5);
    }

    tok = advance(tok);

	return tok;
}


// Processes the procedure declaration
aToken_type proc_declaration(aToken_type tok, int l){

    //return if there are no procedures
	if(tok.t != procsym){
		return tok;
	}

    //parse procedures
	while(tok.t == procsym){
        //store location in code array
        int px = cx;
		tok = advance(tok);

		if(tok.t != identsym){
			error(4);
		}

		//add procedure to symbol table
		int proc_pos = symctr;
        put_symbol(3, tok.val.identifier, 0, l, 0);

		tok = advance(tok);

		if(tok.t != semicolonsym){
			error(5);
		}

		tok = advance(tok);

        //parse the body of the procedure
		tok = block(tok, l+1);

		//update procedure symbol with code address
		update_address(proc_pos, px);

		if(tok.t != semicolonsym){
			error(5);
		}

        //emit return instruction
        emit(OPR, 0, 0);

		tok = advance(tok);
	}

    return tok;
}

// Addresses multiple declared statements
aToken_type statement(aToken_type tok, int l){

    int ctemp, cx1, cx2;

    if(tok.t == identsym){

        //check to make sure ident is a valid variable symbol stored in symbol table
        int sym_pos = find_symbol_kind(tok.val.identifier, 2);

        tok = advance (tok);

        if(tok.t != becomessym && tok.t != semicolonsym){
            //printf("token = %d\n", tok.t);
            error(3);
        }

        tok = advance(tok);
        tok = expression(tok, l);

        //LOD and STO instructions must use lex-level difference
        emit(STO, l - symbol_level(sym_pos), symbol_address(sym_pos));

        return tok;
    }

    if(tok.t == callsym){
        tok = advance(tok);

        int sym_pos = find_symbol_kind(tok.val.identifier, 3);

        // covered by find_symbol_kind above
        /*if(tok.t != identsym){
            error(14);
        }*/

        emit(CAL, l-symbol_level(sym_pos), symbol_address(sym_pos));

        tok = advance(tok);
    }

    if(tok.t == beginsym){
        tok = advance(tok);
        tok = statement(tok, l);
        //emit(STO, 0, 4);

        do{
            tok = advance(tok);
            tok = statement(tok, l);

        }while(tok.t == semicolonsym);

        tok = statement(tok, l);

        //printf("%d \n", tok.t); //** this is where error throws if contents of else aren't there twice. Maybe a do while will fix?

        if(tok.t != endsym){
            error(17);
        }

        tok = advance(tok);
        //emit(STO, 0, 4);

        return tok;

    }

    if(tok.t == ifsym){
        tok = advance(tok);
        tok = condition(tok, l);

        if(tok.t != thensym){
            error(16);
        }else{
            tok = advance(tok);
        }

        ctemp = cx;
        emit(JPC, 0, 0);
        tok = statement(tok, l);
        codeArray[ctemp].m = cx;

        return tok;
    }

    /*if (tok.t == elsesym){ //**************************
        tok = advance(tok);
        ctemp = cx;
        emit(JPC, 0, 0);
        tok = statement(tok);
        codeArray[ctemp].m = cx;

        tok = advance(tok);
        ctemp = cx;
        emit(JPC, 0, 0);
        tok = statement(tok);
        codeArray[ctemp].m = cx;

        return tok;
    }                      //**************************
    */
    if(tok.t == whilesym){
        cx1 = cx;
        tok = advance(tok);
        tok = condition(tok, l);
        cx2 = cx;

        emit(JPC, 0, 0);

        if(tok.t != dosym){
            error(18);
        }else{
            tok = advance(tok);
        }

        tok = statement(tok, l);
        emit(JMP, 0, cx1);
        codeArray[cx2].m = cx;

        return tok;
    }

    if(tok.t == readsym){
        tok = advance(tok);

        emit(SIO, 0, 1);

        if (tok.t != identsym){
            error(27);
        }

        //check to make sure ident is a valid variable symbol stored in symbol table
        int sym_pos = find_symbol_kind(tok.val.identifier, 2);
        emit(STO, l - symbol_level(sym_pos), symbol_address(sym_pos));

        tok = advance(tok);

        if (tok.t != semicolonsym){
            error(10);
        }

        tok = statement(tok, l);
        return tok;
    }

    if(tok.t == writesym){
        tok = advance(tok);

        if (tok.t != identsym){
            error(28);
        }

        //check to make sure ident is a valid variable symbol stored in symbol table
        int sym_pos = find_symbol(tok.val.identifier);
        if (symbol_kind(sym_pos) != 1 && symbol_kind(sym_pos) != 2){
            error(28);
        }
                           //find_symbol_kind(tok.val.identifier, 2); // *********************** needs to take in constants too, according to example

        //emit instructions to load the identifier & print it
        //need to update L
        emit (LOD, l - symbol_level(sym_pos), symbol_address(sym_pos));
        emit(SIO, 0, 0);

        tok = advance(tok);

        //if (tok.t != semicolonsym){
        //    error(10);
        //}

        tok = factor(tok, l);
        return tok;
    }

    return tok;
}

// Processes a condition
aToken_type condition(aToken_type tok, int l){

	if(tok.t == oddsym){
		tok = advance(tok);
		emit(OPR, 0, 6);
		tok = expression(tok, l);

	}else{
		tok = expression(tok, l);

		int rel_sym = relation(tok);
		tok = advance(tok);

		tok = expression(tok, l);

        emit(OPR, 0, rel_sym);
	}
	return tok;
}

int relation(aToken_type tok){

	switch(tok.t){
		case eqsym:
		    return 8;
			break;
		case neqsym:
		    return 9;
			break;
		case lessym:
		    return 10;
			break;
		case leqsym:
		    return 11;
			break;
		case gtrsym:
		    return 12;
			break;
		case geqsym:
		    return 13;
			break;
		default:
			error(20);
			break;
	}
}

// Handles expressions
aToken_type expression(aToken_type tok, int l){

	int addop;

    if(tok.t == plussym || tok.t == minussym){
        addop = tok.t;
        tok = advance(tok);
        tok = term(tok, l);

        if(addop == minussym){
            emit(OPR, 0, NEG);
        }

        if(addop == plussym){
            emit(OPR, 0, ADD);
        }

    }else{
        tok = term(tok, l);
    }

	while(tok.t == plussym || tok.t == minussym){
		addop = tok.t;
		tok = advance(tok);
		tok = term(tok, l);

		if (addop == plussym){
            emit(OPR, 0, ADD); // addition

        }else{
            emit(OPR, 0, SUB); // subtraction
        }
	}

	return tok;

}

// Handles the terms
aToken_type term(aToken_type tok, int l){
	int mulop;

	tok = factor(tok, l);

	while(tok.t == multsym || tok.t == slashsym){
		mulop = tok.t;
		tok = advance(tok);
		tok = factor(tok, l);

		if (mulop == multsym){
            emit(OPR, 0, MUL);
		}else{
            emit(OPR, 0, DIV);
		}
	}

	return tok;
}

// Processes a factor
aToken_type factor(aToken_type tok, int l){

		if (tok.t == identsym){
            //check to make sure ident is a valid variable symbol stored in symbol table
            int sym_pos = find_symbol_kind(tok.val.identifier, -1);
            tok = advance(tok);
            // you need to distinguish between a constant and a variable
            if (symbol_kind(sym_pos) == 1){
                emit(LIT, 0, tok.val.number);
            }
            else {
                //need to update L
                emit(LOD, l - symbol_level(sym_pos), symbol_address(sym_pos));
            }
		}else if(tok.t == numbersym){
            emit(LIT, 0, (tok.val).number);
            tok = advance(tok);
		}else if(tok.t == lparentsym){
            tok = advance(tok);
            tok = expression(tok, l);

            if (tok.t != rparentsym){
                error(22);
            }
            tok = advance(tok);

		}else{
			return tok;
		}

		return tok;
}

void put_symbol(int kind, char* name, int num, int level, int modifier){
    //bounds check
    if (symctr >= MAX_SYMBOL_TABLE_SIZE){
        error(26);
    }
    //store symbol type
    symbol_table[symctr].kind = kind;
    //store string name
    symbol_table[symctr].name = (char*) malloc(sizeof(char) * MAX_IDENT_LENGTH);
    strcpy( symbol_table[symctr].name, name);
    //store int value
    symbol_table[symctr].val = num;
    //store int lexicographical level
    symbol_table[symctr].level = level;
    //store modifier
    symbol_table[symctr].addr = modifier;
    //increase symbol counter
    symctr++;
}

void update_address(int pos, int modifier){
    symbol_table[pos].addr = modifier;
}

int find_symbol(char* identstr){
    int i;
    for (i=0; i < symctr; i++){
        if (strcmp(identstr, symbol_table[i].name) == 0){
            return i;
        }
    }
    return -1;
}

int symbol_kind(int symbol_pos){
    return symbol_table[symbol_pos].kind;
}

int symbol_level(int symbol_pos){
    return symbol_table[symbol_pos].level;
}

int symbol_address(int symbol_pos){
    return symbol_table[symbol_pos].addr;
}

//takes an ident string and an expected kind (-1 for any)
//returns position in symbol table or throws error if not found
int find_symbol_kind(char* identstr, int kind){
    //check to make sure ident is a valid variable stored in symbol table
        int symbol_pos = find_symbol(identstr);
        if (symbol_pos == -1){
            error(11);
        } else if (kind != -1 && symbol_kind(symbol_pos) != kind){
            error(12);
        }
        return symbol_pos;
}

void emit(int op, int l, int m){
    if(cx > MAX_CODE_LENGTH){
        error(26);
    }else{
        (codeArray[cx]).op= op; // opcode
        (codeArray[cx]).l = l; // lexicographical level
        (codeArray[cx]).m = m; // modifier
        cx++;
    }
}

void print_pm0(FILE* outFile){
    int i;

    for(i = 0; i < cx; i++){
        fprintf(outFile, "%d %d %d\n", (codeArray[i]).op, (codeArray[i]).l, (codeArray[i]).m);
    }
}
void print_pm0_screen(){
    int i;
    for(i = 0; i < cx; i++){
        printf("%d %d %d\n", (codeArray[i]).op, (codeArray[i]).l, (codeArray[i]).m);
    }
}

void print_symboltable(){
    printf("ROW\tKIND\tNAME\tVAL\tL\tM\n---------------------------------------------\n");
    int i;
    for (i=0; i < symctr; i++){
        symbol s = symbol_table[i];
        printf("%d\t%d\t%s\t%d\t%d\t%d\n", i, s.kind, s.name, s.val, s.level, s.addr);
    }

}
