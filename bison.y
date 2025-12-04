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


    List *variables;
    List *initialize();
    void insert(Variable *data);
    Variable *search(char *name);
    void clean();

    int yylex(void);
    void yyerror(const char *s);
%}

/* Definição dos tipos possíveis para os terminais e não-terminais. */

%union {
    int inteiro;
    double real;

    Types type;
    Flex flex;
    Variable var;
}

/* Definição dos não-terminais. */

%nterm <var> S

/* Definição dos terminais. */

%token <type> INTEIRO REAL LISTAINT LISTAREAL
%token <flex> var_name

/* Gramática. */

%%

S: INTEIRO var_name {
        Variable *v = (Variable *)malloc(sizeof(Variable));
        v->name = strdup($2.name);
        v->type = $1;
        v->size = $2.length;
        v->initialized = 0;
        v->data = NULL;

        insert(v);
    };

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
    while (n && n->variable->name != name) {
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
            if (n->variable->initialized) {
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

/* Implementação das funções padrões. */

void yyerror(const char *s) {
    fprintf(stderr, "O seguinte erro ocorreu: %s.\n", s);
}

int main() {
    variables = initialize();
    if (!variables) return 1;

    yyparse();
    clean();
    return 0;
}