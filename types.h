#ifndef TYPES
#define TYPES

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

/*
Tipos possíveis para as variáveis do programa.
*/
typedef enum {
    T_INTEIRO,
    T_REAL,
    T_LISTAINT,
    T_LISTAREAL,
    T_UNTYPED, // Usado somente pelo bison.
} Types;

/*
Estrutura para representar uma variável. O dado só deve ser alocado quando for atribuído, e deve ser convertido para o tipo por meio do "cast".
*/
typedef struct {
    char *name;
    Types type;
    int initialized; // Indica se a variável já foi inicializada.
    int size; // Utilizado apenas em casos de lista, pois representa o tamanho delas.
    void *data; // O dado em si.
} Variable;

/*
Estrutura auxiliar para o Flex retornar o nome de uma variável, e no caso de listas, o tamanho delas também.
*/
typedef struct {
    char *name;
    int length;
} Flex;

#endif