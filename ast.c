#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "types.h"
#include "variables.h"

extern List *variables;

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
        perror("malloc() failed");
        exit(1);
    }

    memset(n, 0, sizeof(Node));
    n->type = t;
    return n;
}

void add_child(Node *n, Node *child) {
    if (!n || !child) return;
    if (n->type != NODE_BLOCK) {
        fprintf(stderr, "add_child(): the node n must be of type NODE_BLOCK.\n");
        exit(1);
    }

    Node **new_cmds = (Node **)malloc(sizeof(Node *) * (n->block.count + 1));
    if (!new_cmds) {
        perror("malloc() failed");
        exit(1);
    }

    new_cmds[0] = child;
    memcpy(new_cmds + 1, n->block.cmds, sizeof(Node *) * n->block.count);
    n->block.count++;

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
    if (!n1 || !n2) {
        fprintf(stderr, "join_blocks(): the nodes must exist.\n");
        exit(1);
    }
    if (n1->type != NODE_BLOCK || n2->type != NODE_BLOCK) {
        fprintf(stderr, "join_blocks(): the nodes must be of type NODE_BLOCK.\n");
        exit(1);
    }

    int count = n1->block.count + n2->block.count;
    Node **new_cmds = (Node **)malloc(sizeof(Node *) * count);
    if (!new_cmds) {
        perror("malloc() failed");
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
    if (!cmds) {
        fprintf(stderr, "make_block(): the cmds parameter must exist.\n");
        exit(1);
    }

    Node *n = alloc_node(NODE_BLOCK);
    n->block.cmds = cmds;
    n->block.count = count;
    return n;
}


Node *make_decl(Types type, const char *name, int size) {
    Node *n = alloc_node(NODE_DECL);
    n->decl.vartype = type;
    n->decl.name = strdup(name);
    n->decl.size = size;
    return n;
}

Node *make_assign(Node *expr, Node *var) {
    if (!expr || !var) {
        fprintf(stderr, "make_assign(): the nodes must exist.\n");
        exit(1);
    }
    if (var->type != NODE_VAR) {
        fprintf(stderr, "make_assing(): the node var must be of type NODE_VAR.\n");
        exit(1);
    }

    Node *n = alloc_node(NODE_ASSIGN);
    n->assign.expr = expr;
    n->assign.var = var;
    return n;
}

Node *make_if(Node *cond, Node *then_block, Node *else_block) {
    if (!cond || !then_block) {
        fprintf(stderr, "make_if(): the nodes (cond and then_block) must exist.\n");
        exit(1);
    }
    if (cond->type != NODE_RELOP) {
        fprintf(stderr, "make_if(): the node cond must be of type NODE_RELOP.\n");
        exit(1);
    }

    Node *n = alloc_node(NODE_IF);
    n->ifnode.cond = cond;
    n->ifnode.then_block = then_block;
    n->ifnode.else_block = else_block;
    return n;
}

Node *make_while(Node *cond, Node *body) {
    if (!cond || !body) {
        fprintf(stderr, "make_while(): the nodes must exist.\n");
        exit(1);
    }
    if (cond->type != NODE_RELOP) {
        fprintf(stderr, "make_while(): the node cond must be of type NODE_RELOP.\n");
        exit(1);
    }

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

int eval_index(Index index) {
    if (index.type == INTEGER) {
        return index.value.integer;
    } else {
        Variable *var = search(variables, index.value.name);
        if (!var) {
            fprintf(stderr, "eval_index(): undeclared variable '%s'.\n", index.value.name);
            exit(1);
        }
        if (!var->initialized) {
            fprintf(stderr, "eval_index(): variable '%s' not initialized.\n", index.value.name);
            exit(1);
        }
        return *(int *)var->data;
    }
}

Node *make_var(const char *name, Index index) {
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

Node *make_write(const char *string, Node *var) {
    Node *n = alloc_node(NODE_WRITE);
    if (!string) {
        n->writenode.string = NULL;
    } else {
        n->writenode.string = strdup(string);
    }
    n->writenode.var = var;
    return n;
}

Node *make_read(Node *var) {
    if (!var) {
        fprintf(stderr, "make_read(): the node must exist.\n");
        exit(1);
    }
    if (var->type != NODE_VAR) {
        fprintf(stderr, "make_read(): the node var must be of type NODE_VAR.\n");
        exit(1);
    }

    Node *n = alloc_node(NODE_READ);
    n->readnode.var = var;
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
            free_node(n->assign.expr);
            free_node(n->assign.var);
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
            break;
        case NODE_BINOP:
            free_node(n->binop.left);
            free_node(n->binop.right);
            break;
        case NODE_RELOP:
            free_node(n->relop.left);
            free_node(n->relop.right);
            break;
        case NODE_WRITE:
            if (n->writenode.string) {
                free(n->writenode.string);
            }
            free_node(n->writenode.var);
            break;
        case NODE_READ:
            free_node(n->readnode.var);
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
            if (!v->data) {
                perror("malloc() failed");
                exit(1);
            }
        }

        if (val.type == T_INTEIRO) {
            *(int *)v->data = val.v.i;
        } else {
            *(int *)v->data = (int)val.v.d;
        }

        v->initialized = 1;
        return 1;
    } else if (v->type == T_REAL) {
        if (!v->data) {
            v->data = malloc(sizeof(double));
            if (!v->data) {
                perror("malloc() failed");
                exit(1);
            }
        }

        if (val.type == T_INTEIRO) {
            *(double *)v->data = (double)val.v.i;
        } else {
            *(double *)v->data = val.v.d;
        }

        v->initialized = 1;
        return 1;
    } else if (v->type == T_LISTAINT) {
        if (!v->data) {
            v->data = malloc(sizeof(int) * v->size);
            if (!v->data) {
                perror("malloc() failed");
                exit(1);
            }
        }

        if (index < 0 || index >= v->size) {
            fprintf(stderr, "set_variable_value_from_eval(): index out of range.\n");
            exit(1);
        }

        if (val.type == T_INTEIRO) {
            ((int *)v->data)[index] = val.v.i;
        } else {
            ((int *)v->data)[index] = (int)val.v.d;
        }

        v->initialized = 1;
        return 1;
    } else if (v->type == T_LISTAREAL) {
        if (!v->data) {
            v->data = malloc(sizeof(double) * v->size);
            if (!v->data) {
                perror("malloc() failed");
                exit(1);
            }
        }

        if (index < 0 || index >= v->size) {
            fprintf(stderr, "set_variable_value_from_eval(): index out of range.\n");
            exit(1);
        }

        if (val.type == T_INTEIRO) {
            ((double *)v->data)[index] = (double)val.v.i;
        } else {
            ((double *)v->data)[index] = val.v.d;
        }

        v->initialized = 1;
        return 1;
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
            Variable *v = search(variables, n->var.name);
            if (!v) {
                fprintf(stderr, "eval_node() - NODE_VAR: undeclared variable '%s'.\n", n->var.name);
                exit(1);
            }
            if (!v->initialized) {
                fprintf(stderr, "eval_node() - NODE_VAR: variable '%s' not initialized.\n", n->var.name);
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
                int index = eval_index(n->var.index);
                if (index < 0 || index >= v->size) {
                    fprintf(stderr, "eval_node() - NODE_VAR: index out of range.\n");
                    exit(1);
                }

                if (v->type == T_LISTAINT) {
                    r.type = T_INTEIRO;
                    r.v.i = ((int *)v->data)[index];
                } else {
                    r.type = T_REAL;
                    r.v.d = ((double *)v->data)[index];
                }
                return r;
            } else {
                fprintf(stderr, "eval_node() - NODE_VAR: unsupported variable type.\n");
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
                    default: r.v.i = left.v.i / right.v.i;
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
                    default: r.v.d = ld / rd;
                }
            }
            return r;
        }
        case NODE_RELOP:
        {
            EvalResult left = eval_node(n->relop.left);
            if (n->relop.op == R_NAO) {
                r.v.i = !left.v.i;
            } else {
                EvalResult right = eval_node(n->relop.right);
                double ld = (left.type == T_INTEIRO) ? (double)left.v.i : left.v.d;
                double rd = (right.type == T_INTEIRO) ? (double)right.v.i : right.v.d;

                switch (n->relop.op) {
                    case R_MAQ: r.v.i = ld > rd; break;
                    case R_MAI: r.v.i = ld >= rd; break;
                    case R_MEQ: r.v.i = ld < rd; break;
                    case R_MEI: r.v.i = ld <= rd; break;
                    case R_IGU: r.v.i = ld == rd; break;
                    case R_DIF: r.v.i = ld != rd; break;
                    case R_OU: r.v.i = ld || rd; break;
                    case R_E: r.v.i = ld && rd; break;
                    default: r.v.i = 0;
                }
            }
            r.type = T_INTEIRO;
            return r;
        }
        default:
            fprintf(stderr, "eval_node(): unsupported node type '%d'.\n", n->type);
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
            Variable *v = create_var(n->decl.name, n->decl.vartype, n->decl.size);
            insert(variables, v);
            break;
        }
        case NODE_ASSIGN:
        {
            EvalResult val = eval_node(n->assign.expr);
            Variable *v = search(variables, n->assign.var->var.name);
            if (!v) {
                fprintf(stderr, "execute_node() - NODE_ASSIGN: undeclared variable '%s'.\n", n->assign.var->var.name);
                exit(1);
            }

            int index = eval_index(n->assign.var->var.index);
            if (!set_variable_value_from_eval(v, val, index)) {
                fprintf(stderr, "execute_node() - NODE_ASSIGN: assignment failed (unsupported type).\n");
                exit(1);
            }
            break;
        }
        case NODE_IF:
        {
            EvalResult cond = eval_node(n->ifnode.cond);
            if (cond.v.i) {
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
                if (!cond.v.i) break;
                execute_node(n->whilenode.body);
            }
            break;
        }
        case NODE_WRITE:
        {
            if (!n->writenode.var) {
                printf("%s\n", n->writenode.string);
            } else {
                EvalResult val = eval_node(n->writenode.var);
                if (val.type == T_INTEIRO) {
                    if (n->writenode.string) {
                        printf("%s%d\n", n->writenode.string, val.v.i);
                    } else {
                        printf("%d\n", val.v.i);
                    }
                } else {
                    if (n->writenode.string) {
                        printf("%s%f\n", n->writenode.string, val.v.d);
                    } else {
                        printf("%f\n", val.v.d);
                    }
                }
            }
            break;
        }
        case NODE_READ:
        {
            EvalResult val;
            Variable *v = search(variables, n->readnode.var->var.name);
            if (!v) {
                fprintf(stderr, "execute_node() - NODE_READ: undeclared variable '%s'.\n", n->readnode.var->var.name);
                exit(1);
            }

            if (v->type == T_INTEIRO || v->type == T_LISTAINT) {
                val.type = T_INTEIRO;
                scanf("%d", &val.v.i);
            } else {
                val.type = T_REAL;
                scanf("%f", &val.v.d);
            }

            int index = eval_index(n->readnode.var->var.index);
            if (!set_variable_value_from_eval(v, val, index)) {
                fprintf(stderr, "execute_node() - NODE_READ: assignment failed (unsupported type).\n");
                exit(1);
            }
            break;
        }
        default:
            fprintf(stderr, "execute_node(): unsupported node type '%d'.\n", n->type);
            exit(1);
    }
}
