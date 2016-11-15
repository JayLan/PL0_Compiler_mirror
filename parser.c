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
aToken_type block(aToken_type tok);
aToken_type const_declaration(aToken_type tok);
aToken_type var_declaration(aToken_type tok);
aToken_type proc_declaration(aToken_type tok);
aToken_type statement(aToken_type tok);
aToken_type condition(aToken_type tok);
aToken_type expression(aToken_type tok);
aToken_type term(aToken_type tok);
aToken_type factor(aToken_type tok);
void relation(aToken_type tok);
aToken_type advance(aToken_type tok);

void put_symbol(int kind, char* name, int num, int level, int modifier);
int find_symbol(char* identstr);
int symbol_kind(int symbol_pos);
int symbol_level(int symbol_pos);
int symbol_address(int symbol_pos);
void emit (int op, int l, int m);
void print_pm0(FILE* outFile);

/*SYMBOL  TABLE and PM0 CODE ARRAY AVAIL TO ALL PARSER FUNCTIONS*/
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction codeArray [MAX_CODE_LENGTH];

/* Global counters */
static int cx = 0;
static int symctr = 0;


// *** main() is located in compile.c ***
// *** PARSER ***
aToken_type advance(aToken_type tok){

    tok = nextToken();
    return tok;
}

// comments
void program(aToken_type tok){

    //printf("program function | token is %d\n", tok.t);
    advance(tok);
    //printf("advancing at start | token is %d\n", tok.t);

	tok = block(tok);

	if(tok.t != periodsym){
		error(9);
	}

	emit(SIO, 0, 2);

	printf("No errors, the program is syntactically correct.\n");

}

// Processes code block declarations
aToken_type block(aToken_type tok){

    //printf("block function | token is %d\n", tok.t);
    tok = const_declaration(tok);
    tok = var_declaration(tok);
    tok = proc_declaration(tok);
    tok = statement(tok);

	return tok;
}

// Handles declarations of constants
aToken_type const_declaration(aToken_type tok){

    //printf("const_declaration function | token is %d\n", tok.t);

	if(tok.t != constsym){
        //printf("tok != constsym, returning from const_declaration\n");
		return tok;
	}


	do {
        //printf("looping in const_declaration\n");
        tok = advance(tok);

		if(tok.t != identsym){
            //printf("not an ident sym- exiting from const_declaration with error\n");
			error(4);
		}
        char *id = (tok.val).identifier;
		tok = advance(tok);

		if(tok.t != eqsym){
            //printf("not an eq sym- exiting from const_declaration with error\n");
			error(3);
		}
		tok = advance(tok);

		if(tok.t != numbersym){
			error(2);
		}
		put_symbol(1, id, tok.t, 0, 0);
		tok = advance(tok);

	}while (tok.t == commasym);

	if(tok.t != semicolonsym){
		error(5);
	}

	tok = advance(tok);

	return tok;

}

// Handles declarations of variables
aToken_type var_declaration(aToken_type tok){

    //printf("var_declaration function | token is %d\n", tok.t);

	if(tok.t != varsym){
        //printf("tok not varsym- returning from var_declaration\n");
        return tok;
	}

    int num_vars = 0;

	do{
        //printf("looping in var_declaration | token is %d\n", tok.t);
        tok = advance(tok);
        //printf("advancing... | token is %d\n", tok.t);
        if (tok.t != identsym){
            //printf("tok not identsym, exiting with error\n");
            error(4);
        }

        num_vars++;
        put_symbol(2, tok.val.identifier, 0, 0, (3 + num_vars));
        tok = advance(tok);

	}while(tok.t == commasym);

    if(tok.t != semicolonsym){
        error(5);
    }

    tok = advance(tok);
    emit(INC, 0, 4 + num_vars);

	return tok;
}

// Processes the process declaration
aToken_type proc_declaration(aToken_type tok){

	if(tok.t != procsym){
		return tok;
	}

	while(tok.t == procsym){
		tok = advance(tok);

		if(tok.t != identsym){
			error(4);
		}
		tok = advance(tok);

		if(tok.t != semicolonsym){
			error(5);
		}
		tok = advance(tok);
		tok = block(tok);

		if(tok.t != semicolonsym){
			error(5);
		}
		tok = advance(tok);
	}

    return tok;
}

// Addresses multiple declared statements
aToken_type statement(aToken_type tok){

    int ctemp, cx1, cx2;

    if(tok.t == identsym){
        tok = advance (tok);

        if(tok.t != becomessym && tok.t != semicolonsym){
            error(3);
        }

        tok = advance(tok);
        tok = expression(tok);

        return tok;
    }

    if(tok.t == callsym){
        tok = advance(tok);

        if(tok.t != identsym){
            error(14);
        }

        tok = advance(tok);
    }

    if(tok.t == beginsym){

        tok = advance(tok);
        tok = statement(tok);
        emit(STO, 0, 4);

        do{
            tok = advance(tok);
            tok = statement(tok);

        }while(tok.t == semicolonsym);

        tok = statement(tok);

        if(tok.t != endsym){
            error(17);
        }

        tok = advance(tok);
        emit(STO, 0, 4);

        return tok;

    }

    if(tok.t == ifsym){
        tok = advance(tok);
        tok = condition(tok);

        if(tok.t != thensym){
            error(16);
        }else{
            tok = advance(tok);
        }

        ctemp = cx;
        emit(JPC, 0, 0);
        tok = statement(tok);
        symbol_table[ctemp].addr = cx;

        return tok;
    }

    if(tok.t == whilesym){
        cx1 = cx;
        tok = advance(tok);
        tok = condition(tok);
        cx2 = cx;

        emit(JPC, 0, 0);

        if(tok.t != dosym){
            error(18);
        }else{
            tok = advance(tok);
        }

        tok = statement(tok);
        emit(JMP, 0, cx1);
        symbol_table[cx2].addr = cx;

        return tok;
    }

    if(tok.t == readsym){
        tok = advance(tok);

        emit(SIO, 0, 1);

        if (tok.t != identsym){
            error(0);
        }

        tok = advance(tok);

        if (tok.t != semicolonsym){
            error(0);
        }

        tok = statement(tok);
        return tok;
    }

    if(tok.t == writesym){
        tok = advance(tok);

        emit(SIO, 0, 0);

        if (tok.t != identsym){
            error(0);
        }

        tok = advance(tok);

        if (tok.t != semicolonsym){
            error(0);
        }

        tok = factor(tok);
        return tok;
}

// Processes a condition
aToken_type condition(aToken_type tok){

	if(tok.t == oddsym){
		tok = advance(tok);
		emit(OPR, 0, 6);
		tok = expression(tok);

	}else{
		tok = expression(tok);
		relation(tok);
		tok = advance(tok);
		tok = expression(tok);

	}
	return tok;
}

void relation(aToken_type tok){

	switch(tok.t){
		case eqsym:
		    emit(OPR, 0, 8);
			break;
		case neqsym:
		    emit(OPR, 0, 9);
			break;
		case lessym:
		    emit(OPR, 0, 10);
			break;
		case leqsym:
		    emit(OPR, 0, 11);
			break;
		case gtrsym:
		    emit(OPR, 0, 12);
			break;
		case geqsym:
		    emit(OPR, 0, 13);
			break;
		default:
			error(20);
			exit(1);
			break;
	}
}

// Handles expressions
aToken_type expression(aToken_type tok){

	int addop;

    if(tok.t == plussym || tok.t == minussym){
        addop = tok.t;
        tok = advance(tok);
        tok = term(tok);

        if(addop == minussym){
            emit(OPR, 0, NEG);
        }

        if(addop == plussym){
            emit(OPR, 0, ADD);
        }

    }else{
        tok = term(tok);
    }

	while(tok.t == plussym || tok.t == minussym){
		addop = tok.t;
		tok = advance(tok);
		tok = term(tok);

		if (addop == plussym){
            emit(OPR, 0, ADD); // addition

        }else{
            emit(OPR, 0, SUB); // subtraction
        }
	}

	return tok;

}

// Handles the terms
aToken_type term(aToken_type tok){
	int mulop;

	tok = factor(tok);

	while(tok.t == multsym || tok.t == slashsym){
		mulop = tok.t;
		tok = advance(tok);
		tok = factor(tok);

		if (mulop == multsym){
            emit(OPR, 0, MUL);
		}else{
            emit(OPR, 0, DIV);
		}
	}

	return tok;
}

// Processes a factor
aToken_type factor(aToken_type tok){

		if (tok.t == identsym){
            tok = advance(tok);
            emit(LOD, 0, 4);
		}else if(tok.t == numbersym){
            emit(LIT, 0, (tok.val).number);
            tok = advance(tok);
		}else if(tok.t == lparentsym){
            tok = advance(tok);
            tok = expression(tok);

            if (tok.t != rparentsym){
                error(22);
				exit(1);
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
    symbol_table[symctr].name = malloc(sizeof(char) * MAX_IDENT_LENGTH);
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

int find_symbol(char* identstr){
    int i;
    for (i=0; i < symctr; i++){
        if (strcmp(identstr, symbol_table[i].name) == 0){
            return i;
        }
    }
    return 0;
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
        printf("%d %d %d\n", (codeArray[i]).op, (codeArray[i]).l, (codeArray[i]).m);
        fprintf(outFile, "%d %d %d\n", (codeArray[i]).op, (codeArray[i]).l, (codeArray[i]).m);
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
