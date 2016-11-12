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
#include "tokensss.h"
#include "errorsss.h"
#include "lexersss.h"

void program(token_type tok);
token_type block(token_type tok);
token_type const_declaration(token_type tok);
token_type var_declaration(token_type tok);
token_type proc_declaration(token_type tok);
token_type statement(token_type tok);
token_type condition(token_type tok);
token_type expression(token_type tok);
token_type term(token_type tok);
token_type factor(token_type tok);
void relation(token_type tok); // return type?
token_type advance(token_type tok);

/*THIS IS THE GLOBAL TOKEN STORAGE AVAILABLE TO ALL PARSER FUNCTIONS*/
token_type tokArray [TOKEN_ARRAY_SIZE];
static int lexCtr = 0;

int main() {

    //Declare and Initialize Variables:
    //aToken_type *toke;
    token_type tok;
    int i = 0, j = 0;
    //int lexCtr = 0;

    //Open and Declare File Pointers:
    FILE *source;
    FILE *clean;

    source = fopen("tester2.txt", "r");
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

    //Close File Pointers
    fclose(source);
    fclose(clean);

    //Have Clean point to the new, clean file:
    clean = fopen("clean.pl0", "r");

    if (clean == NULL){
        printf("Unable to open file.\n");
    }

   while(true)
    {
        //Get the tokens from the clean file.
        aToken_type* toke = getNextToken(clean);

        //Store value of the token (the sym type only)
        //Need to alter array typing in order to take in entire token
        tokArray[i] = toke->t;

        // halt if nullsym is returned
        if ( toke->t == 1 )
            break;

        //This frees toke, so it no longer points to the token
        freeToken(toke);

        //Increment i for array storage purposes
        i++;
    }

    //This is just here to test if the tokens were stored in tokArray correctly
    while (j < i){
        printf("%d ", tokArray [j]);
        j++;
    }

    printf("\n");

    //Close the cleanFile pointer
    fclose(clean);

    //Set tok to first token
    tok = tokArray[0];

    //Go through parser
    program(tok);

return 0;
}


// *** PARSER ***

//advance stores next token in tok
token_type advance(token_type tok){

    lexCtr++;

    //Check to make sure we aren't accessing passed array limits
    if (lexCtr < TOKEN_ARRAY_SIZE){
        tok = tokArray[lexCtr];
    }

    //Commented out print to check which token is now in tok
    //Used to identify where possible errors take place in the parser
    printf("%d \n", tok);
    return tok;
}

// comments
void program(token_type tok){

	tok = block(tok);

	if(tok != periodsym){
		error(9);
		exit(1);
	}

	printf("No errors, the program is syntactically correct.\n");

}

// comments
token_type block(token_type tok){

    if (tok == constsym){
        tok = const_declaration(tok);
    }

    if (tok == varsym){
        tok = var_declaration(tok);
    }

    if (tok == procsym){
        tok = proc_declaration(tok);
    }

    tok = statement(tok);

	return tok;
}

// comments
token_type const_declaration(token_type tok){

	if(tok != constsym){
		return tok;
	}
	tok = advance(tok);

	while(tok != commasym){

		if(tok != identsym){
			error(4);
			exit(1);
		}
		tok = advance(tok);

		if(tok != eqsym){
			error(3);
			exit(1);
		}
		tok = advance(tok);

		// are we testing fornumbersym
		// or if its a digit [isn't this tested in lexer
		// and it assigns numbersym?]
		if(tok != numbersym){
			error(2);
			exit(1);
		}
		tok = advance(tok);
	}

	if(tok != semicolonsym){
		error(5);
		exit(1);
	}

	tok = advance(tok);

	return tok;

}

// comments
token_type var_declaration(token_type tok){

	tok = advance(tok);

    if (tok != identsym){
        error(4);
        exit(1);
    }

    tok = advance(tok);

    if (tok == commasym){
        tok = var_declaration(tok);
        return tok;
    }

    if (tok != semicolonsym){
        error(5);
        exit(1);
    }

        tok = advance(tok);


	return tok;
}

// comments
token_type proc_declaration(token_type tok){

	if(tok != procsym){
		return tok;
	}
	while(tok == procsym){
		tok = advance(tok);

		if(tok != identsym){
			error(4);
			exit(1);
		}
		tok = advance(tok);

		if(tok != semicolonsym){
			error(5);
			exit(1);
		}
		tok = advance(tok);
		tok = block(tok);

		if(tok != semicolonsym){
			error(5);
			exit(1);
		}
		tok = advance(tok);
	}

    return tok;
}

// comments
token_type statement(token_type tok){

	/*switch(tok){
		case identsym:
			tok = advance(tok);

			if(tok != becomessym){
				error(0); // !!! input the error code !!!
				exit(1);
			}
			tok = advance(tok);
			tok = expression(tok);
			return tok;
			break;

		case callsym:
			tok = advance(tok);

			if(tok != identsym){
				error(14);
				exit(1);
			}
			tok = advance(tok);
			return tok;
			break;

		case beginsym:
			tok = advance(tok);
			tok = statement(tok);

			while(tok == semicolonsym){
				tok = advance(tok);
				tok = statement(tok);
			}

			if(tok != endsym){
				error(0); // !!! input the error code !!!
				exit(1);
			}
			tok = advance(tok);
			return tok;
			break;

		case ifsym:
			tok = advance(tok);
			tok = condition(tok);

			if(tok != thensym){
				error(16);
				exit(1);
			}
			tok = advance(tok);
			return tok;
			break;

		case whilesym:
			tok = advance(tok);
			tok = condition(tok);

			if(tok != dosym){
				error(18);
				exit(1);
			}
			tok = advance(tok);
			tok = statement(tok);
			return tok;
			break;

		default:
			//error(7); // no error, epsilon is a valid string for statement
			break;

		} // END switch
*/
    if (tok == identsym){
        tok = advance (tok);

        if (tok != becomessym){
            error(0); // !!! input the error code !!!
            exit(1);
        }

        tok = advance(tok);
        tok = expression(tok);
    }

     if (tok == callsym){
        tok = advance(tok);

        if (tok != identsym){
            error(14);
            exit(1);
        }

        tok = advance(tok);
    }

     if (tok == beginsym){
        tok = advance(tok);
        tok = statement(tok);

        if (tok != semicolonsym){
            error(5);
			exit(1);
        }

        tok = advance(tok);

        if (tok != semicolonsym){
            tok = statement(tok);
        }

        if (tok != endsym){
            error(17); // !!! input the error code !!!
            exit(1);
        }

        tok = advance(tok);
    }

     if (tok == ifsym){
        tok = advance(tok);
        tok = condition(tok);

        if (tok != thensym){
            error(16);
            exit(1);
        }

        tok = advance(tok);
        tok = statement(tok);
    }

     if (tok == whilesym){
        tok = advance(tok);
        tok = condition(tok);

        if (tok != dosym){
            error(18);
            exit(1);
        }

        tok = advance(tok);
        tok = statement(tok);
    }

    else{
        return tok;
    }


    return tok;
}

// comments
token_type condition(token_type tok){

	if(tok == oddsym){
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

void relation(token_type tok){

	switch(tok){
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
token_type expression(token_type tok){

	/*if(tok != plussym || tok != minussym){
		return tok;
	}
    */

    if (tok == plussym || tok == minussym){
        tok = advance(tok);
    }

    tok = term(tok);

	while(tok == plussym || tok == minussym){
		tok = advance(tok);
		tok = term(tok);
	}

return tok;
}

// comments
token_type term(token_type tok){
	tok = factor(tok);

	while(tok == multsym || tok == slashsym){
		tok = advance(tok);
		tok = factor(tok);
	}

	return tok;
}

// comments
token_type factor(token_type tok){

		if (tok == identsym){
            tok = advance(tok);
		}

		else if (tok == numbersym){
            tok = advance(tok);
		}

		else if (tok == lparentsym){
            tok = advance(tok);
            tok = expression(tok);

            if (tok != rparentsym){
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
