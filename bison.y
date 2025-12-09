%{
    #include "ast.h"
    #include "types.h"
    #include "variables.h"

    List *variables;

    int yylex(void);
    void yyerror(const char *s);
%}

/* Definition of possible types for terminals and non-terminals. */

%union {
    int integer;
    double real;
    char *string;

    Node *node;

    RelOp operand;
    Types type;
    Flex flex;
    Variable var;
}

/* Definition of non-terminals. */

%type <node> start program statements names algorithm commands assignment input input_vars output out_string if loop expression lower middle high complex relational high_relational

%type <type> type
%type <operand> r_operators b_operators

/* Definition of terminals. */

%token PROGRAMA FIMPROG ATRIB LEIA ESCREVA SE ENTAO SENAO FIMSE ENQUANTO FACA FIMENQ

%token <integer> N_INT
%token <real> N_REAL
%token <string> STRING

%token <operand> NAO MAQ MAI MEQ MEI IGU DIF OU E
%token <type> INTEIRO REAL LISTAINT LISTAREAL
%token <flex> VAR_NAME

/* Grammar. */

%%

start:
    PROGRAMA program FIMPROG
    {
        execute_node($2);
        free_node($2);
        $$ = $2;
    };

program:
    statements algorithm
    {
        $$ = join_blocks($1, $2);
    };

statements:
    type names statements {
        change_untypeds($2->block.cmds, $2->block.count, $1);
        $$ = join_blocks($2, $3);
    }
    | type names
    {
        change_untypeds($2->block.cmds, $2->block.count, $1);
        $$ = $2;
    };

type:
    INTEIRO
    | REAL
    | LISTAINT
    | LISTAREAL;

names:
    VAR_NAME ',' names
    {
        $$ = $3;
        add_child($$, make_decl(T_UNTYPED, $1.name, $1.length));
    }
    | VAR_NAME
    {
        Node **cmds = (Node **)malloc(sizeof(Node *));
        cmds[0] = make_decl(T_UNTYPED, $1.name, $1.length);

        $$ = make_block(cmds, 1);
    };

algorithm:
    commands algorithm
    {
        $$ = join_blocks($1, $2);
    }
    | commands
    {
        $$ = $1;
    };

commands:
    assignment
    {
        $$ = $1;
    }
    | input
    {
        $$ = $1;
    }
    | output
    {
        $$ = $1;
    }
    | if
    {
        $$ = $1;
    }
    | loop
    {
        $$ = $1;
    };

assignment:
    VAR_NAME ATRIB expression
    {
        $$ = make_assign($3, make_var($1.name, $1.length));
    };

input:
    LEIA input_vars
    {
        $$ = $2;
    };

input_vars:
    VAR_NAME ',' input_vars
    {
        $$ = $3;
        add_child($$, make_read(make_var($1.name, $1.length)));
    }
    | VAR_NAME
    {
        Node **cmds = (Node **)malloc(sizeof(Node *));
        cmds[0] = make_read(make_var($1.name, $1.length));

        $$ = make_block(cmds, 1);
    };

output:
    ESCREVA out_string
    {
        $$ = $2;
    };

out_string:
    VAR_NAME
    {
        $$ = make_write(NULL, make_var($1.name, $1.length));
    }
    | STRING
    {
        $$ = make_write($1, NULL);
    }
    | STRING ',' VAR_NAME
    {
        $$ = make_write($1, make_var($3.name, $3.length));
    };

if:
    SE complex ENTAO algorithm FIMSE
    {
        $$ = make_if($2, $4, NULL);
    }
    | SE complex ENTAO algorithm SENAO algorithm FIMSE
    {
        $$ = make_if($2, $4, $6);
    };

loop:
    ENQUANTO complex FACA algorithm FIMENQ
    {
        $$ = make_while($2, $4);
    };

expression:
    lower
    {
        $$ = $1;
    };

lower:
    lower '+' middle
    {
        $$ = make_binop(OP_ADD, $1, $3);
    }
    | lower '-' middle
    {
        $$ = make_binop(OP_SUB, $1, $3);
    }
    | middle
    {
        $$ = $1;
    };

middle:
    middle '*' high
    {
        $$ = make_binop(OP_MUL, $1, $3);
    }
    | middle '/' high
    {
        $$ = make_binop(OP_DIV, $1, $3);
    }
    | high
    {
        $$ = $1;
    };

high:
    N_INT
    {
        $$ = make_int($1);
    }
    | N_REAL
    {
        $$ = make_real($1);
    }
    | VAR_NAME
    {
        $$ = make_var($1.name, $1.length);
    }
    | '(' lower ')'
    {
        $$ = $2;
    };

complex:
    relational b_operators high_relational
    {
        $$ = make_relop($2, $1, $3);
    }
    | high_relational
    {
        $$ = $1;
    };

relational:
    expression r_operators expression
    {
        $$ = make_relop($2, $1, $3);
    };

high_relational:
    NAO '(' relational ')'
    {
        $$ = make_relop($1, $3 ,NULL);
    }
    | '(' relational ')'
    {
        $$ = $2;
    }
    | relational
    {
        $$ = $1;
    };

r_operators:
    MAQ
    | MAI
    | MEQ
    | MEI
    | IGU
    | DIF;

b_operators:
    OU
    | E;
%%

/* Implementation of standard functions. */

void yyerror(const char *s) {
    fprintf(stderr, "An error has occurred: %s.\n", s);

    clean(variables);
    free(variables);
}

int main() {
    variables = initialize();
    if (!variables) return 1;

    yyparse();

    clean(variables);
    free(variables);
    return 0;
}