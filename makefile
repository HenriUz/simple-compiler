all:
	bison --report=all --color=[always] --warning=all -d -Wcounterexamples bison.y
	flex lexical.lex
	gcc lex.yy.c bison.tab.c -o compiler

clean:
	rm -f lex.yy.c bison.output bison.tab.* compiler