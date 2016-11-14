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

//Included Libraries
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "error.h"
#include "lexer.h"
#include "vm.h"

// *** do we need to include this? ***#include "vmsss.h"

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
 int kind; // const = 1, var = 2, proc = 3
 char name[12]; // name up to 11 chars
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
void relation(aToken_type tok); // return type?
aToken_type advance(aToken_type tok);

void put_symbol(int kind, char name [], int num, int level, int modifier);
void emit (int op, int l, int m);
void print_pm0();

/*THIS IS THE GLOBAL TOKEN STORAGE AVAILABLE TO ALL PARSER FUNCTIONS*/

aToken_type* tok; // *** can this be deleted? ***

aToken_type tokArray [TOKEN_ARRAY_SIZE];
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
instruction codeArray [MAX_CODE_LENGTH];
static int lexCtr = 0;
static int cx = 0;
static int symctr = 0;

//moved main() to compile.c

// *** PARSER ***
aToken_type advance(aToken_type tok){

    lexCtr++;

    //Check to make sure we aren't accessing passed array limits
    if (lexCtr < TOKEN_ARRAY_SIZE){
        tok = tokArray[lexCtr];
    }

    //Commented out print to check which token is now in tok
    //Used to identify where possible errors take place in the parser
    printf("%d \n", tok.t);
    return tok;
}

// comments
void program(aToken_type tok){

	tok = block(tok);

	if(tok.t != periodsym){
		error(9);
		exit(1);
	}

	emit(SIO, 0, 2);

	printf("No errors, the program is syntactically correct.\n");

}

// comments
aToken_type block(aToken_type tok){

    tok = const_declaration(tok);
    tok = var_declaration(tok);
    tok = proc_declaration(tok);
    tok = statement(tok);

	return tok;
}

// comments
aToken_type const_declaration(aToken_type tok){

	if(tok.t != constsym){
		return tok;
	}


	do {
        tok = advance(tok);

		if(tok.t != identsym){
			error(4);
			exit(1);
		}

        char *id = (tok.val).identifier;
		tok = advance(tok);

		if(tok.t != eqsym){
			error(3);
			exit(1);
		}

		tok = advance(tok);

		// are we testing fornumbersym
		// or if its a digit [isn't this tested in lexer
		// and it assigns numbersym?]
		if(tok.t != numbersym){
			error(2);
			exit(1);
		}

		put_symbol(1, id, tok.t, 0, 0);

		tok = advance(tok);

	} while (tok.t == commasym);


	if(tok.t != semicolonsym){
		error(5);
		exit(1);
	}

	tok = advance(tok);

	return tok;

}

// comments
aToken_type var_declaration(aToken_type tok){

	if (tok.t != varsym){
        return tok;
	}

    int num_vars = 0;

	do {

        tok = advance(tok);
        if (tok.t != identsym){
            error(4);
            exit(1);
        }

        num_vars++;

        put_symbol(2, tok.val.identifier, 0, 0, (3 + num_vars));

        tok = advance(tok);

	} while (tok.t == commasym);

    if (tok.t != semicolonsym){
        error(5);
        exit(1);
    }

    tok = advance(tok);
    emit(INC, 0, 4 + num_vars);

	return tok;
}

// comments
aToken_type proc_declaration(aToken_type tok){

	if(tok.t != procsym){
		return tok;
	}
	while(tok.t == procsym){
		tok = advance(tok);

		if(tok.t != identsym){
			error(4);
			exit(1);
		}
		tok = advance(tok);

		if(tok.t != semicolonsym){
			error(5);
			exit(1);
		}
		tok = advance(tok);
		tok = block(tok);

		if(tok.t != semicolonsym){
			error(5);
			exit(1);
		}
		tok = advance(tok);
	}

    return tok;
}

// comments
aToken_type statement(aToken_type tok){

    int ctemp, cx1, cx2;

    if (tok.t == identsym){
        tok = advance (tok);

        if (tok.t != becomessym && tok.t != semicolonsym){
            error(0); // !!! input the error code !!!
            exit(1);
        }

        tok = advance(tok);
        tok = expression(tok);

        return tok;
    }

    if (tok.t == callsym){
        tok = advance(tok);

        if (tok.t != identsym){
            error(14);
            exit(1);
        }

        tok = advance(tok);
    }

    if (tok.t == beginsym){

        tok = advance(tok);
        tok = statement(tok);
        emit(STO, 0, 4);

        do {
            tok = advance(tok);
            tok = statement(tok);
        } while (tok.t == semicolonsym);

        if (tok.t != endsym){
            error(17); // !!! input the error code !!!
            exit(1);
        }

        tok = advance(tok);

        emit(STO, 0, 4);
        return tok;

    }

    if (tok.t == ifsym){
        tok = advance(tok);
        tok = condition(tok);

        if (tok.t != thensym){
            error(16);
            exit(1);
        }

        else{
            tok = advance(tok);
        }

        ctemp = cx;
        emit(JPC, 0, 0);
        tok = statement(tok);
        symbol_table[ctemp].addr = cx;

        return tok;
    }

    if (tok.t == whilesym){
        cx1 = cx;
        tok = advance(tok);
        tok = condition(tok);
        cx2 = cx;

        emit(JPC, 0, 0);

        if (tok.t != dosym){
            error(18);
            exit(1);
        }

        else{
            tok = advance(tok);
        }

        tok = statement(tok);
        emit(JMP, 0, cx1);
        symbol_table[cx2].addr = cx;

        return tok;
    }

    if (tok.t == readsym){
        tok = advance(tok);
        tok = statement(tok);
        return tok;
    }

    if (tok.t == writesym){
        tok = advance(tok);
        emit(SIO, 0, 0);
        tok = factor(tok);
        return tok;
    }

    else{
        return tok;
    }

    return tok;
}

// comments
aToken_type condition(aToken_type tok){

	if(tok.t == oddsym){
		tok = advance(tok);
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
			break;
		case neqsym:
			break;
		case lessym:
			break;
		case leqsym:
			break;
		case gtrsym:
			break;
		case geqsym:
			break;
		default:
			error(20);
			exit(1);
			break;
	}
}

// comments
aToken_type expression(aToken_type tok){

	int addop;

    if (tok.t == plussym || tok.t == minussym){
        addop = tok.t;
        tok = advance(tok);
        tok = term(tok);

        if (addop == minussym){
            emit(OPR, 0, NEG);
        }

        if (addop == plussym){
            emit(OPR, 0, ADD);
        }
    }

    else{
        tok = term(tok);
    }

	while(tok.t == plussym || tok.t == minussym){
		addop = tok.t;
		tok = advance(tok);
		tok = term(tok);
		if (addop == plussym)
            emit(OPR, 0, ADD); // addition
        else
            emit(OPR, 0, SUB); // subtraction
	}

return tok;
}

// comments
aToken_type term(aToken_type tok){
	int mulop;

	tok = factor(tok);

	while(tok.t == multsym || tok.t == slashsym){
		mulop = tok.t;
		tok = advance(tok);
		tok = factor(tok);

		if (mulop == multsym){
            emit(OPR, 0, MUL);
		}

		else{
            emit(OPR, 0, DIV);
		}
	}

	return tok;
}

// comments
aToken_type factor(aToken_type tok){

		if (tok.t == identsym){
            tok = advance(tok);
            emit(LOD, 0, 4);
		}

		else if (tok.t == numbersym){
            emit(LIT, 0, (tok.val).number);
            tok = advance(tok);
		}

		else if (tok.t == lparentsym){
            tok = advance(tok);
            tok = expression(tok);

            if (tok.t != rparentsym){
                error(22);
				exit(1);
            }

            tok = advance(tok);

		}

		else{
			error(23); // !!! check this !!!
			exit(1);
		}

		return tok;
	} // END switch

void put_symbol(int kind, char name [], int num, int level, int modifier){
    (symbol_table [symctr]).kind = kind;
    //(symbol_table [symctr]).name = name;
    (symbol_table [symctr]).val = num;
    (symbol_table [symctr]).level = level;
    (symbol_table [symctr]).addr = modifier;
}

void emit(int op, int l, int m){
    if(cx > MAX_CODE_LENGTH){
        error(26);
        exit(1);
    }
    else{
        (codeArray[cx]).op= op; // opcode
        (codeArray[cx]).l = l; // lexicographical level
        (codeArray[cx]).m = m; // modifier
        cx++;
    }
}

void print_pm0(){
    int i;

    for (i = 0; i < cx; i++){
        printf("%d %d %d\n", (codeArray[i]).op, (codeArray[i]).l, (codeArray[i]).m);
    }

}
