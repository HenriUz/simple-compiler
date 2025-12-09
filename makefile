CC = gcc
CFLAGS_RELEASE =
CFLAGS_DEBUG = -DDEBUG

# Default build: release.

all: release

release: clean
release: CFLAGS = $(CFLAGS_RELEASE)
release: BUILD_DIR = build/compiler
release: compiler

# Debug build: debug.

debug: clean
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: BUILD_DIR = build/debug
debug: compiler

# General rules.

bison.tab.c bison.tab.h: bison.y
	bison --report=all --color=[always] --warning=all -d -Wcounterexamples bison.y

lex.yy.c: lexical.lex bison.tab.h
	flex lexical.lex

compiler: bison.tab.c lex.yy.c types.o ast.o variables.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR) bison.tab.c lex.yy.c types.o ast.o variables.o -lfl

ast.o: ast.c ast.h variables.h types.h
	$(CC) $(CFLAGS) -c ast.c

variables.o: variables.c variables.h types.h
	$(CC) $(CFLAGS) -c variables.c

types.o: types.c types.h
	$(CC) $(CFLAGS) -c types.c

clean:
	rm -f *.o lex.yy.c bison.output bison.tab.*