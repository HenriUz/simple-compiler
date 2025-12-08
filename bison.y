%{
    #include "ast.h"
    #include "types.h"

    typedef struct LNode {
        Variable *variable;
        struct LNode *next;
    } LNode;

    typedef struct List {
        LNode *start;
    } List;


    /* Funções para a lista de variáveis. */
    List *variables;
    List *initialize();
    void insert(Variable *data);
    Variable *search(char *name);
    void clean();

    /* Funções para reduzir as linhas de código dentro da gramática. */
    Variable *create(char *name, Types type, int size);

    int yylex(void);
    void yyerror(const char *s);
%}

/* Definição dos tipos possíveis para os terminais e não-terminais. */

%union {
    int inteiro;
    double real;
    char *cadeia;

    Node *node;

    RelOp op;
    Types type;
    Flex flex;
    Variable var;
}

/* Definição dos não-terminais. */

%type <node> start program statements names algorithm commands assignment input input_vars output out_string if loop expression lower middle high complex relational high_relational

%type <type> type
%type <op> r_operators b_operators

/* Definição dos terminais. */

%token PROGRAMA FIMPROG ":=" LEIA ESCREVA SE ENTAO SENAO FIMSE ENQUANTO FACA FIMENQ

%token <inteiro> N_INT
%token <real> N_REAL
%token <cadeia> STRING

%token <op> NAO MAQ MAI MEQ MEI IGU DIF OU E
%token <type> INTEIRO REAL LISTAINT LISTAREAL
%token <flex> VAR_NAME

/* Gramática. */

%%

start:
    PROGRAMA program FIMPROG
    {
        execute_node($2);
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
        $$ = $1
    };

assignment:
    VAR_NAME ":=" expression
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
        $$ = make_write($1, make_var($1.name, $1.length));
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
        $$ = make_relop($1, $3 NULL);
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

/* Implementação das funções auxiliares (declaradas no início do arquivo). */

List *initialize() {
    List *l = (List *)malloc(sizeof(List));
    if (!l) return NULL;

    l->start = NULL;
    return l;
}

void insert(Variable *data) {
    if (!variables) return;

    LNode *n = (LNode *)malloc(sizeof(LNode));
    if (!n) return;

    n->variable = data;
    n->next = variables->start;
    variables->start = n;
}

Variable *search(char *name) {
    if (!variables) return NULL;

    LNode *n = variables->start;
    while (n && strcmp(n->variable->name, name) != 0) {
        n = n->next;
    }

    if (!n) return NULL;
    return n->variable;
}

void clean() {
    if (!variables) return;

    while (variables->start) {
        LNode *n = variables->start;

        if (n->variable) {
            free(n->variable->name);
            if (n->variable->initialized == TRUE) {
                free(n->variable->data);
            }
            free(n->variable);
        }

        variables->start = n->next;
        free(n);
    }

    free(variables);
    variables = NULL;
}

Variable *create(char *name, Types type, int size) {
    Variable *v = (Variable *)malloc(sizeof(Variable));
    if (!v) return NULL;

    v->name = strdup(name);
    v->type = type;
    v->initialized = FALSE;
    v->size = size;
    v->data = NULL;

    return v;
}

/* Implementação das funções padrões. */

void yyerror(const char *s) {
    fprintf(stderr, "O seguinte erro ocorreu: %s.\n", s);
    clean();
}

int main() {
    variables = initialize();
    if (!variables) return 1;

    yyparse();
    clean();
    return 0;
}