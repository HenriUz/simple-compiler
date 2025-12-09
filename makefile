all: compiler

bison.tab.c bison.tab.h: bison.y
	bison --report=all --color=[always] --warning=all -d -Wcounterexamples bison.y

lex.yy.c: lexical.lex bison.tab.h
	flex lexical.lex

compiler: bison.tab.c lex.yy.c types.o ast.o variables.o
	gcc -o compiler bison.tab.c lex.yy.c types.o ast.o variables.o -lfl

ast.o: ast.c ast.h variables.h types.h
	gcc -c ast.c

variables.o: variables.c variables.h types.h
	gcc -c variables.c

types.o: types.c types.h
	gcc -c types.c

clean:
	rm -f *.o lex.yy.c bison.output bison.tab.*