#ifndef AST_H
#define AST_H

#include "types.h"

/**
 * @enum NodeType
 *
 * @brief Possible types of nodes in the AST.
 */
typedef enum NodeType {
    NODE_BLOCK,     // Lista de declarações.
    NODE_DECL,      // Declaração: tipo nome.
    NODE_ASSIGN,    // Atribuição: nome := expr.
    NODE_IF,        // Se condição then_block else_block.
    NODE_WHILE,     // While condição.
    NODE_EXPR,      // Expressão.
    NODE_INT,       // Int.
    NODE_REAL,      // Real.
    NODE_VAR,       // Acesso a variável.
    NODE_BINOP,     // + - * /.
    NODE_RELOP,     // Operadores relacionais.
} NodeType;

/**
 * @enum BinOp
 *
 * @brief Arithmetic operations.
 */
typedef enum BinOp {
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,
} BinOp;

/**
 * @enum RelOp
 *
 * @brief Relational operations.
 */
typedef enum RelOp {
    R_MAQ, R_MAI, R_MEQ, R_MEI, R_IGU, R_DIF,
} RelOp;

/**
 * @struct EvalResult
 *
 * @brief Possible result for the evaluation of some expression.
 *
 * It does not consider list types, and the boolean type is stored as an integer i.
 */
typedef struct EvalResult {
    Types type;
    union {
        int i;
        double d;
    } v;
} EvalResult;

/**
 * @struct Node
 *
 * @brief Represents a node in the AST.
 *
 * It contains only the type and the data, which in this case is a structure relevant to the type.
 */
typedef struct Node {
    NodeType type;
    union {
        /* Program / block. */
        struct { struct Node **cmds; int count; } block;

        /* Declaration. */
        struct { Types vartype; char *name; int size; } decl;

        /* Assignment. */
        struct { char *name; struct Node *expr; int index; } assign;

        /* If. */
        struct { struct Node *cond; struct Node *then_block; struct Node *else_block; } ifnode;

        /* While. */
        struct { struct Node *cond; struct Node *body; } whilenode;

        /* Literals. */
        int intval;
        double realval;

        /* Variavle acess. */
        struct { char *name; struct Node *index; } var;

        /* Binary op. */
        struct { BinOp op; struct Node *left; struct Node *right; } binop;

        /* Relational op. */
        struct { RelOp op; struct Node *left; struct Node *right; } relop;
    };
} Node;

/**
 * @brief Adds a child node to the specified node.
 *
 * The node specified must be of type NODE_BLOCK, as the child will be saved in cmds.
 *
 * @param n The parent node.
 * @param child The node to be added.
 */
void add_child(Node *n, Node *child);

/**
 * @brief Adds a valid type for decl nodes.
 *
 * @param cmds Vector of pointers to decl nodes.
 * @param length Vector length.
 * @param type Valid type.
 */
void change_untypeds(Node **cmds, int length, Types type);

/**
 * @brief Join the nodes into a single larger one.
 *
 * The nodes must be of type NODE_BLOCK.
 *
 * @param n1 First node.
 * @param n2 Second node.
 *
 * @return The new node.
 */
Node *join_blocks(Node *n1, Node *n2);

/**
 * @brief Creates a node of type NODE_BLOCK.
 *
 * @param cmds Vector of declaration nodes.
 * @param count Number of items in the vector.
 *
 * @return A pointer to the created node.
 */
Node *make_block(Node **cmds, int count);

/**
 * @brief Creates a node of type NODE_DECL.
 *
 * @param t Variable type.
 * @param name Variable name.
 * @param size Vector size (used only if it is a vector type).
 *
 * @return A pointer to the created node.
 */
Node *make_decl(Types t, const char *name, int size);

/**
 * @brief Creates a node of type NODE_ASSIGN.
 *
 * @param name Variable name.
 * @param expr Node representing the expression that will result in the value of the variable.
 * @param index Variable index (used only if it is a vector type).
 *
 * @return A pointer to the created node.
 */
Node *make_assign(const char *name, Node *expr, int index);

/**
 * @brief Creates a node of type NODE_IF.
 *
 * @param cond Node representing the expression that will result in the boolean value of the condition.
 * @param then_block Node representing the code that will be executed if true.
 * @param else_block Node representing the code that will be executed if false (can be NULL).
 *
 * @return A pointer to the created node.
 */
Node *make_if(Node *cond, Node *then_block, Node *else_block);

/**
 * @brief Creates a node of type NODE_WHILE.
 *
 * @param cond Node representing the expression that will result in the boolean value of the condition.
 * @param body Node representing the code that will be executed while true.
 *
 * @return A pointer to the created node.
 */
Node *make_while(Node *cond, Node *body);

/**
 * @brief Creates a node of type NODE_INT.
 *
 * @param v Integer value.
 *
 * @return A pointer to the created node.
 */
Node *make_int(int v);

/**
 * @brief Creates a node of type NODE_REAL.
 *
 * @param v Double value.
 *
 * @return A pointer to the created node.
 */
Node *make_real(double v);

/**
 * @brief Creates a node of type NODE_VAR.
 *
 * @param name Variable name.
 * @param index Node representing the expression that will result in the index of the vector (if is a vector type).
 *
 * @return A pointer to the created node.
 */
Node *make_var(const char *name, Node *index);

/**
 * @brief Creates a node of type NODE_BINOP.
 *
 * @param op Arithmetic operator.
 * @param left Node representing the expression that will result in the value of the left side.
 * @param right Node representing the expression that will result in the value of the right side.
 *
 * @return A pointer to the created node.
 */
Node *make_binop(BinOp op, Node *left, Node *right);

/**
 * @brief Creates a node of type NODE_RELOP.
 *
 * @param op Relational operator.
 * @param left Node representing the expression that will result in the value of the left side.
 * @param right Node representing the expression that will result in the value of the right side.
 *
 * @return A pointer to the created node.
 */
Node *make_relop(RelOp op, Node *left, Node *right);

/**
 * @brief Recursively frees memory.
 *
 * @param n Target node.
 */
void free_node(Node *n);

/**
 * @brief Calculates the value of nodes of type: NODE_INT, NODE_REAL, NODE_VAR, NODE_BINOP, NODE_RELOP.
 *
 * @param n Node to be calculated.
 *
 * @return The result of the calculation.
 */
EvalResult eval_node(Node *n);

/**
 * @brief Execute the corresponding codes (NODE_BLOCK, NODE_DECL, NODE_ASSIGN, NODE_IF, NODE_WHILE).
 *
 * @param n Node representing the code.
 */
void execute_node(Node *n);

#endif // AST_H