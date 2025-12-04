/*
Ideia:

As variáveis serão salvas em uma lista encadeada (mais simples de implementar se for similar a pilha), no qual o nó da lista contém o ponteiro para o próximo, um ponteiro para a estrutura "Variable".
*/

%{
    #include "types.h"

    typedef struct Node {
        Variable *variable;
        struct Node *next;
    } Node;

    typedef struct List {
        Node *start;
    } List;


    /* Funções para a lista de variáveis. */
    List *variables;
    List *initialize();
    void insert(Variable *data);
    Variable *search(char *name);
    void clean();

    /* Funções para reduzir as linhas de código dentro da gramática. */
    Variable *create(char *name, Types type, int size);
    void change_untypeds(Types type);

    int yylex(void);
    void yyerror(const char *s);
%}

/* Definição dos tipos possíveis para os terminais e não-terminais. */

%union {
    int inteiro;
    double real;
    char *cadeia;

    Bool tbool;
    Operators op;
    Types type;
    Flex flex;
    Variable var;
}

/* Definição dos não-terminais. */

%nterm <real> expression lower middle high
%nterm <tbool> complex relational high_relational

%nterm <type> type
%nterm <op> r_operators b_operators

/* Definição dos terminais. */

%token PROGRAMA FIMPROG ":=" LEIA ESCREVA SE ENTAO SENAO FIMSE ENQUANTO FACA FIMENQ

%token <inteiro> N_INT
%token <real> N_REAL
%token <cadeia> STRING

%token <op> .NAO. .MAQ. .MAI. .MEQ. .MEI. .IGU. .DIF. .OU. .E.
%token <type> INTEIRO REAL LISTAINT LISTAREAL
%token <flex> VAR_NAME

/* Gramática. */

%%

start:
    PROGRAMA program FIMPROG;
program:
    statements algorithm;

statements:
    type names { change_untypeds($1); } statements
    | type names { change_untypeds($1); };
type:
    INTEIRO
    | REAL
    | LISTAINT
    | LISTAREAL;
names:
    VAR_NAME ',' {
        Variable *v = create($1.name, T_UNTYPED, $1.length);
        if (!v) {
            yyerror("falha na alocação de memória");
            YYABORT;
        }
        insert(v);
    } names
    | VAR_NAME {
        Variable *v = create($1.name, T_UNTYPED, $1.length);
        if (!v) {
            yyerror("falha na alocação de memória");
            YYABORT;
        }
        insert(v);
    };

algorithm: commands algorithm
         | commands
         ;
commands: assignment
        | input
        | output
        | if
        | loop
        ;
assignment: VAR_NAME ":=" expression ;
input: LEIA names ;
output: ESCREVA out_string ;
out_string: VAR_NAME
          | STRING
          | VAR_NAME ',' STRING
          | STRING ',' VAR_NAME
          ;
if: SE complex ENTAO algorithm FIMSE
  | SE complex ENTAO algorithm SENAO algorithm FIMSE
  ;
loop: ENQUANTO complex FACA algorithm FIMENQ ;

expression: lower { $$ = $1; };
lower:
    lower '+' middle { $$ = $1 + $3; }
    | lower '-' middle { $$ = $1 - $3; }
    | middle { $$ = $1; };
middle:
    middle '*' high { $$ = $1 * $3; }
    | middle '/' high { $$ = $1 / $3; }
    | high { $$ = $1; };
high:
    N_INT { $$ = $1; }
    | N_REAL { $$ = $1; }
    | VAR_NAME {
        Variable *v = search($1.name);
        if (!v) {
            yyerror("uso de variável não declarada");
            YYABORT;
        }

        if (v->initialized == FALSE) {
            yyerror("uso de variável não inicializada");
            YYABORT;
        }

        if (v->type == LISTAINT || v->type == LISTAREAL) {
            if ($1.length < 0) {
                yyerror("um vetor não pode ser usado em uma expressão, tente informar o indíce");
                YYABORT;
            } else {
                $$ = ((double *)v->data)[$1.length];
            }
        } else {
            $$ = *(double *)v->data;
        }
    }
    | '(' lower ')' { $$ = $2; };

complex:
    relational b_operators high_relational {
        switch($2) {
            case OU:
                $$ = $1 || $3;
                break;
            case E:
            default:
                $$ = $1 && $3;
        }
    }
    | high_relational { $$ = $1; };
relational:
    expression r_operators expression {
        switch($2) {
            case MAQ:
                $$ = $1 > $3 ? TRUE : FALSE;
                break;
            case MAI:
                $$ = $1 >= $3 ? TRUE : FALSE;
                break;
            case MEQ:
                $$ = $1 < $3 ? TRUE : FALSE;
                break;
            case MEI:
                $$ = $1 <= $3 ? TRUE : FALSE;
                break;
            case IGU:
                $$ = $1 == $3 ? TRUE : FALSE;
                break;
            case DIF:
            default:
                $$ = $1 != $3 ? TRUE : FALSE;
        }
    };
high_relational:
    .NAO. '(' relational ')' { $$ = !$3; }
    | '(' relational ')' { $$ = $2; }
    | relational { $$ = $1; };
r_operators:
    .MAQ.
    | .MAI.
    | .MEQ.
    | .MEI.
    | .IGU.
    | .DIF.;
b_operators:
     .OU.
    | .E.;
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

    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) return;

    n->variable = data;
    n->next = variables->start;
    variables->start = n;
}

Variable *search(char *name) {
    if (!variables) return NULL;

    Node *n = variables->start;
    while (n && strcmp(n->variable->name, name) != 0) {
        n = n->next;
    }

    if (!n) return NULL;
    return n->variable;
}

void clean() {
    if (!variables) return;

    while (variables->start) {
        Node *n = variables->start;

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

void change_untypeds(Types type) {
    Node *n = variables->start;
    while (n && n->variable->type == T_UNTYPED) {
        n->variable->type = type;
        n = n->next;
    }
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