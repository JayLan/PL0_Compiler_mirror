compiler: compile.o error.o lexer.o parser.o tokens.o
	cc -g -o compile -Wall compile.o error.o lexer.o parser.o tokens.o

compile.o: compile.c
	cc -g -c -Wall compile.c

error.o: error.c error.h
	cc -g -c -Wall error.c

tokens.o: tokens.c tokens.h
	cc -g -c -Wall tokens.c

lexer.o: lexer.c lexer.h
	cc -g -c -Wall lexer.c

parser.o: parser.c parser.h
	cc -g -c -Wall parser.c

clean: 
	rm -f a.out compile.o error.o tokens.o lexer.o parser.o