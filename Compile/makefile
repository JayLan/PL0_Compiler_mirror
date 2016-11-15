compiler: compile.o error.o lexer.o parser.o
	g++ compile.o error.o lexer.o parser.o -o compile

compile.o: compile.c
	g++ -c compile.c

error.o: error.c
	g++ -c error.c

lexer.o: lexer.c
	g++ -c lexer.c

parser.o: parser.c
	g++ -c parser.c
	