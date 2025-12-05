#include "ast.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern Variable *search(char *name);
extern void insert(Variable *data);
extern Variable *create(char *name, Types type, int size);

/**
 * @brief Create a new node.
 *
 * @param t Node type.
 *
 * @return A pointer to the created node.
 */
static Node *alloc_node(NodeType t) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        perror("malloc");
        exit(1);
    }

    memset(n, 0, sizeof(Node));
    n->type = t;
    return n;
}

void add_child(Node *n, Node *child) {
    if (!n || !child) return;
    if (n->type != NODE_BLOCK) return;

    Node **new_cmds = (Node **)malloc(sizeof(Node *) * (n->block.count + 1));
    if (!new_cmds) return;

    memcpy(new_cmds, n->block.cmds, sizeof(Node *) * n->block.count);
    new_cmds[(n->block.count)++] = child;

    free(n->block.cmds);
    n->block.cmds = new_cmds;
}

void change_untypeds(Node **cmds, int length, Types type) {
    if (!cmds) return;

    for (int i = 0; i < length; i++) {
        if (cmds[i] && cmds[i]->type == NODE_DECL) {
            cmds[i]->decl.vartype = type;
        }
    }
}

Node *join_blocks(Node *n1, Node *n2) {
    if (!n1 || !n2 || n1->type != NODE_BLOCK || n2->type != NODE_BLOCK) {
        fprintf(stderr, "join_blocks(): nós inválidos.");
        exit(1);
    }

    int count = n1->block.count + n2->block.count;
    Node **new_cmds = (Node **)malloc(sizeof(Node *) * count);
    if (!new_cmds) {
        perror("malloc");
        exit(1);
    }

    memcpy(new_cmds, n1->block.cmds, sizeof(Node *) * n1->block.count);
    memcpy(new_cmds + n1->block.count, n2->block.cmds, sizeof(Node *) * n2->block.count);

    free(n1->block.cmds);
    free(n1);
    free(n2->block.cmds);
    free(n2);

    return make_block(new_cmds, count);
}

Node *make_block(Node **cmds, int count) {
    Node *n = alloc_node(NODE_BLOCK);
    n->block.cmds = cmds;
    n->block.count = count;
    return n;
}


Node *make_decl(Types t, const char *name, int size) {
    Node *n = alloc_node(NODE_DECL);
    n->decl.vartype = t;
    n->decl.name = strdup(name);
    n->decl.size = size;
    return n;
}

Node *make_assign(const char *name, Node *expr, int index) {
    Node *n = alloc_node(NODE_ASSIGN);
    n->assign.name = strdup(name);
    n->assign.expr = expr;
    n->assign.index = index;
    return n;
}

Node *make_if(Node *cond, Node *then_block, Node *else_block) {
    Node *n = alloc_node(NODE_IF);
    n->ifnode.cond = cond;
    n->ifnode.then_block = then_block;
    n->ifnode.else_block = else_block;
    return n;
}

Node *make_while(Node *cond, Node *body) {
    Node *n = alloc_node(NODE_WHILE);
    n->whilenode.cond = cond;
    n->whilenode.body = body;
    return n;
}

Node *make_int(int v) {
    Node *n = alloc_node(NODE_INT);
    n->intval = v;
    return n;
}

Node *make_real(double v) {
    Node *n = alloc_node(NODE_REAL);
    n->realval = v;
    return n;
}

Node *make_var(const char *name, Node *index) {
    Node *n = alloc_node(NODE_VAR);
    n->var.name = strdup(name);
    n->var.index = index;
    return n;
}

Node *make_binop(BinOp op, Node *left, Node *right) {
    Node *n = alloc_node(NODE_BINOP);
    n->binop.op = op;
    n->binop.left = left;
    n->binop.right = right;
    return n;
}

Node *make_relop(RelOp op, Node *left, Node *right) {
    Node *n = alloc_node(NODE_RELOP);
    n->relop.op = op;
    n->relop.left = left;
    n->relop.right = right;
    return n;
}

void free_node(Node *n) {
    if (!n) return;
    switch (n->type){
        case NODE_BLOCK:
            if (n->block.cmds) {
                for (int i = 0; i < n->block.count; i++) {
                    free_node(n->block.cmds[i]);
                }
                free(n->block.cmds);
            }
            break;
        case NODE_DECL:
            free(n->decl.name);
            break;
        case NODE_ASSIGN:
            free(n->assign.name);
            free_node(n->assign.expr);
            break;
        case NODE_IF:
            free_node(n->ifnode.cond);
            free_node(n->ifnode.then_block);
            free_node(n->ifnode.else_block);
            break;
        case NODE_WHILE:
            free_node(n->whilenode.cond);
            free_node(n->whilenode.body);
            break;
        case NODE_VAR:
            free(n->var.name);
            free_node(n->var.index);
            break;
        case NODE_BINOP:
            free_node(n->binop.left);
            free_node(n->binop.right);
            break;
        case NODE_RELOP:
            free_node(n->relop.left);
            free_node(n->relop.right);
            break;
        case NODE_INT:
        case NODE_REAL:
        default:
            break;
    }
    free(n);
}

/**
 * @brief Initializes a variable structure.
 *
 * @param v Pointer to the structure to be initialized.
 * @param val Structure with the value.
 * @param index Variable index (used only if it is a vector type).
 *
 * @return Returns 1 if OK, and 0 if it fails.
 */
static int set_variable_value_from_eval(Variable *v, EvalResult val, int index) {
    if (!v) return 0;
    if (v->type == T_INTEIRO) {
        if (!v->data) {
            v->data = malloc(sizeof(int));
            if (!v->data) return 0;
        }

        if (val.type == T_INTEIRO) {
            *(int *)v->data = val.v.i;
        } else {
            *(int *)v->data = (int)val.v.d;
        }

        v->initialized = TRUE;
        return 1;
    } else if (v->type == T_REAL) {
        if (!v->data) {
            v->data = malloc(sizeof(double));
            if (!v->data) return 0;
        }

        if (val.type == T_INTEIRO) {
            *(double *)v->data = (double)val.v.i;
        } else {
            *(double *)v->data = val.v.d;
        }

        v->initialized = TRUE;
        return 1;
    } else if (v->type == T_LISTAINT) {
        if (!v->data) {
            v->data = malloc(sizeof(int) * v->size);
            if (!v->data) return 0;
        }

        if (index < 0 || index >= v->size) return 0;
        if (val.type == T_INTEIRO) {
            ((int *)v->data)[index] = val.v.i;
        } else {
            ((int *)v->data)[index] = (int)val.v.d;
        }
    } else if (v->type == T_LISTAREAL) {
        if (!v->data) {
            v->data = malloc(sizeof(double) * v->size);
            if (!v->data) return 0;
        }

        if (index < 0 || index >= v->size) return 0;
        if (val.type == T_INTEIRO) {
            ((double *)v->data)[index] = (double)val.v.i;
        } else {
            ((double *)v->data)[index] = val.v.d;
        }
    }
    return 0;
}

EvalResult eval_node(Node *n) {
    EvalResult r;
    if (!n) { r.type = T_REAL; r.v.d = 0.0; return r; }

    switch (n->type) {
        case NODE_INT:
            r.type = T_INTEIRO;
            r.v.i = n->intval;
            return r;
        case NODE_REAL:
            r.type = T_REAL;
            r.v.d = n->realval;
            return r;
        case NODE_VAR:
        {
            Variable *v = search(n->var.name);
            if (!v) {
                fprintf(stderr, "Erro: variável '%s' não declarada.\n", n->var.name);
                exit(1);
            }
            if (!v->initialized) {
                fprintf(stderr, "Erro: variável '%s' não inicializada.\n", n->var.name);
                exit(1);
            }

            if (v->type == T_INTEIRO) {
                r.type = T_INTEIRO;
                r.v.i = *(int *)v->data;
                return r;
            } else if (v->type == T_REAL) {
                r.type = T_REAL;
                r.v.d = *(double *)v->data;
                return r;
            } else if (v->type == T_LISTAINT || v->type == T_LISTAREAL) {
                if (!n->var.index) {
                    fprintf(stderr, "Erro: acesso a vetor sem índice.\n");
                    exit(1);
                }

                EvalResult idx = eval_node(n->var.index);
                int pos = (idx.type == T_INTEIRO) ? idx.v.i : (int)idx.v.d;
                if (pos < 0 || pos >= v->size) {
                    fprintf(stderr, "Erro: índice fora do interval.\n");
                    exit(1);
                }

                if (v->type == T_INTEIRO) {
                    r.type = T_INTEIRO;
                    r.v.i = ((int *)v->data)[pos];
                } else {
                    r.type = T_REAL;
                    r.v.d = ((double *)v->data)[pos];
                }
                return r;
            } else {
                fprintf(stderr, "Erro: tipo não suportado em eval_node(Node *).\n");
                exit(1);
            }
        }
        case NODE_BINOP:
        {
            EvalResult left = eval_node(n->binop.left);
            EvalResult right = eval_node(n->binop.right);
            if (left.type == T_INTEIRO && right.type == T_INTEIRO) {
                r.type = T_INTEIRO;
                switch (n->binop.op) {
                    case OP_ADD: r.v.i = left.v.i + right.v.i; break;
                    case OP_SUB: r.v.i = left.v.i - right.v.i; break;
                    case OP_MUL: r.v.i = left.v.i * right.v.i; break;
                    case OP_DIV:
                    default: r.v.i = left.v.i / right.v.i; break;
                }
            } else {
                double ld = (left.type == T_INTEIRO) ? (double)left.v.i : left.v.d;
                double rd = (right.type == T_INTEIRO) ? (double)right.v.i : right.v.d;
                r.type = T_REAL;
                switch (n->binop.op) {
                    case OP_ADD: r.v.d = ld + rd; break;
                    case OP_SUB: r.v.d = ld - rd; break;
                    case OP_MUL: r.v.d = ld * rd; break;
                    case OP_DIV:
                    default: r.v.d = ld / rd; break;
                }
            }
            return r;
        }
        case NODE_RELOP:
        {
            EvalResult left = eval_node(n->relop.left);
            EvalResult right = eval_node(n->relop.right);
            double ld = (left.type == T_INTEIRO) ? (double)left.v.i : left.v.d;
            double rd = (right.type == T_INTEIRO) ? (double)right.v.i : right.v.d;

            r.type = T_INTEIRO;
            switch (n->relop.op) {
                case R_MAQ: r.v.i = ld > rd; break;
                case R_MAI: r.v.i = ld >= rd; break;
                case R_MEQ: r.v.i = ld < rd; break;
                case R_MEI: r.v.i = ld <= rd; break;
                case R_IGU: r.v.i = ld == rd; break;
                case R_DIF: r.v.i = ld != rd; break;
                default: r.v.i = 0;
            }
            return r;
        }
        default:
            fprintf(stderr, "eval_node: Nó de expressão inesperado %d.\n", n->type);
            exit(1);
    }
}

void execute_node(Node *n) {
    if (!n) return;
    switch (n->type) {
        case NODE_BLOCK:
            for (int i = 0; i < n->block.count; i++) {
                execute_node(n->block.cmds[i]);
            }
            break;
        case NODE_DECL:
        {
            Variable *v = create(n->decl.name, n->decl.vartype, n->decl.size);
            if (!v) {
                fprintf(stderr, "Erro: create() falhou.\n");
                exit(1);
            }

            insert(v);
            break;
        }
        case NODE_ASSIGN:
        {
            EvalResult val = eval_node(n->assign.expr);
            Variable *v = search(n->assign.name);
            if (!v) {
                fprintf(stderr, "Erro: variável '%s' não declarada.\n", n->assign.name);
                exit(1);
            }

            if (!set_variable_value_from_eval(v, val, n->assign.index)) {
                fprintf(stderr, "Erro: atribuição falhou (tipo ou lista não suportado).\n");
                exit(1);
            }
            break;
        }
        case NODE_IF:
        {
            EvalResult cond = eval_node(n->ifnode.cond);
            int c = (cond.type == T_INTEIRO ? cond.v.i : (cond.v.d != 0.0));
            if (c) {
                execute_node(n->ifnode.then_block);
            } else if (n->ifnode.else_block) {
                execute_node(n->ifnode.else_block);
            }
            break;
        }
        case NODE_WHILE:
        {
            while (1) {
                EvalResult cond = eval_node(n->whilenode.cond);
                int c = (cond.type == T_INTEIRO ? cond.v.i : (cond.v.d != 0.0));
                if (!c) break;
                execute_node(n->whilenode.body);
            }
            break;
        }
        default:
            fprintf(stderr, "execute_node(): nó de comando inesperado %d.\n", n->type);
            exit(1);
    }
}