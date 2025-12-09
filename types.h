#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief Relational and boolean operations.
 */
typedef enum RelOp {
    R_MAQ, R_MAI, R_MEQ, R_MEI, R_IGU, R_DIF, R_NAO, R_OU, R_E,
} RelOp;

/**
 * @enum Types
 *
 * @brief Represents the possible types for variables.
 *
 * The T_UNTYPED type is only used by bison because of multiple declarations on a single line.
 */
typedef enum  Types {
    T_INTEIRO,
    T_REAL,
    T_LISTAINT,
    T_LISTAREAL,
    T_UNTYPED,
} Types;

/**
 * @struct Flex
 *
 * @brief Helper for passing the VAR_NAME token from flex to bison.
 *
 * The length field is used to pass the size of the vector, in the case of declarations, and the index being accessed in other cases.
 *
 * The variable field has the same function as length, but it is used when the index provided is a variable.
 */
typedef struct  Flex {
    char *name;
    int length;
    char *variable;
} Flex;

/**
 * @struct Variable
 *
 * @brief Represents a variable.
 *
 * The data field should only be used if the initialized field is 1, and it must be converted to the type in question.
 *
 * The size field is only used if the variable is a vector, as it indicates the allocated size.
 */
typedef struct Variable {
    char *name;
    Types type;
    int initialized;
    int size;
    void *data;
} Variable;

/**
 * @brief Initializes a Variable structure.
 *
 * @param name Variable name.
 * @param type Variable type.
 * @param size Variable size (only if it's a vector).
 *
 * @return A pointer to the created structure.
 */
Variable *create_var(char *name, Types type, int size);

#endif