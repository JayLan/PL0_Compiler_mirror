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

void program();
void block();
void const_declaration();
void var_declaration();
void proc_declaration();
void statement();
void condition();
void expression();
void term();
void factor();
token_type relation(token_type); // return type?
void advance();

/*THIS IS THE GLOBAL TOKEN STORAGE AVAILABLE TO ALL PARSER FUNCTIONS*/
aToken_type* tok;

int main() {

    //Declare and Initialize Variables:
    //aToken_type *toke;

    //Open and Declare File Pointers:
    FILE *source;
    FILE *clean;

    source = fopen("tester.txt", "r");
    clean = fopen("clean.pl0", "w");

    //Check if pointers are valid:
    if (source == NULL){
        printf("Unable to open file.\n");
    }

    if (clean == NULL){
        printf("Unable to open file.\n");
    }

    //Remove Comments from Source File:
    removeComments(source, clean);

    fclose(source);
    fclose(clean);

    //Have Clean point to the new, clean file:
    clean = fopen("clean.pl0", "r");

    while(true)
    {
        //Get the tokens from the clean file.
        //toke = getNextToken(clean); --changed to use the global tok and functions from tokens.h
        tok = nextToken();

        // halt if nullsym is returned
        if ( tok->t == 1 )
            break;

        //Display the appropriate token, then free it
        displayToken(tok);
        freeToken(tok);
    }

    //Back to the beginning
    rewindParseTokens();

    //Close the cleanFile pointer
    fclose(clean);
}


// *** PARSER ***

//advance stores next token in tok
void advance(){
    tok = nextToken();
}

// comments
void program(){
	block();
	if(tok->t != periodsym){
		error(9);
	}
	advance();

}

// comments
void block(){
	const_declaration();
	var_declaration();
	proc_declaration();
	statement();
}

// comments
void const_declaration(){
	if(tok->t != constsym){
		return;
	}
	advance();

	while(tok->t != commasym){
		if(tok->t != identsym){
			error(4);
		}
		advance();

		if(tok->t != eqsym){
			error(3);
		}
		advance();

        //NOTE: There is a c library <ctype.h> used to determine the type of
        //      character. It has a function called isdigit() that might
        //      be able to solve this error.
		if(tok->t != NUMBER){
			error(2);
		}
		advance();
	}

	if(tok->t != semicolonsym){
		error(5);
	}

	advance();

}

// comments
void var_declaration(){
	if(tok->t != varsym){
		return;
	}
	advance();

	while(tok->t != commasym){
		if(tok->t != identsym){
			error(4);
		}
		advance();
	}

	if(tok->t != semicolonsym){
		error(5);
	}

	advance();
}

// comments
void proc_declaration(){

	if(tok->t != procsym){
		return;
	}
	while(tok->t == procsym){
		advance();

		if(tok->t != identsym){
			error(4);
		}
		advance();

		if(tok->t != semicolonsym){
			error(5);
		}
		advance();
		block();

		if(tok->t != semicolonsym){
			error(5);
		}
		advance();
	}

}

// comments
void statement(){

/*

!!! do we need this? I think we are testing it in the switch !!!

	if(tok->t != identsym){
		error(); // !!! input the error code !!!
	}

*/

	switch(tok->t){
		case identsym:
			advance();

			//NOTE: Just changed "becomesym" to "becomessym"
			if(tok->t != becomessym){
				error(); // !!! input the error code !!!
			}
			advance();
			expression();
			break;

		case callsym:
			advance();
			if(tok->t != identsym){
				error(14);
			}
			advance();
			break;

		case beginsym:
			advance();
			statement();

			while(tok->t == semicolonsym){
				advance();
				statement();
			}
			if(tok->t != endsym){
				error(); // !!! input the error code !!!
			}
			advance();
			break;

		case ifsym:
			advance();
			condition();

			if(tok->t != thensym){
				error(16);
			}
			advance();
			break;

		case whilesym:
			advance();
			condition();
			if(tok->t != dosym){
				error(18);
			}
			advance();
			statement();
			break;

		default:
			error(7); // !!! check this !!!
			break;

		} // END switch

}

// comments
void condition(){
	if(tok->t == oddsym){
		advance();
		expression();
	}else{
		expression();
		if(tok != relation(tok->t)){
			error(20);

		}
		advance();
		expression();
	}
}

token_type relation(token_type tok){

	switch(tok){
		case eqsym:
			return eqsym;
			break;
		case neqsym:
			return neqsym;
			break;
		case lessym:
			return lessym;
			break;
		case leqsym:
			return leqsym;
			break;
		case gtrsym:
			return gtrsym;
			break;
		case geqsym:
			return geqsym;
			break;
		default:
			error(20);
			break;
	}

}

// comments
void expression(){
	if(tok->t != plussym || tok->t != minussym){
		return;
	}

	term();

	while(tok->t == plussym || tok->t == minussym){
		advance();
		term();
	}

}

// comments
void term(){
	factor(); // check on this

	while(tok->t == multsym || tok->t == slashsym){
		advance();
		factor();
	}
}

// comments
void factor(){
	switch(tok->t){

		case identsym:
			advance();
			break;

		// !!! need to fix this: !!!
		case NUMBER:
			advance();
			break;

        //NOTE: Just changed "(" into lparentsym.
		case lparentsym:
			advance();
			expression();
			if(tok->t != ")" ){
				error(22);
			}
			advance();
			break;

		default:
			error(23); // !!! check this !!!
			break;

	} // END switch


}





