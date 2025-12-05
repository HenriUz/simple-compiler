all: compiler

bison.tab.c bison.tab.h: bison.y
	bison --report=all --color=[always] --warning=all -d -Wcounterexamples bison.y

lex.yy.x: lexical.lex bison.tab.h
	flex lexical.lex

compiler: bison.tab.c lex.yy.c ast.o
	gcc -o compiler bison.tab.c lex.yy.c ast.o -lfl

ast.o: ast.c ast.h
	gcc -c ast.c

clean:
	rm -f *.o lex.yy.c bison.output bison.tab.* compiler