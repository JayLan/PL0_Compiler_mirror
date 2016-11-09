lextest: driver.o lex.yy.o util.o
  cc -g -o lextest driver.o lex.yy.o util.o

driver.o: driver.c tokens.h util.h
  cc -g -c driver.c

lex.yy.o: lex.yy.c tokens.h util.h
  cc -g -c lex.yy.c

lex.yy.c: pl0.lex
  lex pl0.lex

util.o: util.c util.h
  cc -g -c util.c

clean: 
  rm -f a.out util.o driver.o lex.yy.o lex.yy.c 