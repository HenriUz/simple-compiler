#ifndef VARIABLES_H
#define VARIABLES_H

#include "types.h"

/**
 * @struct ListNode
 *
 * @brief Represents a variable in the list.
 *
 * Contains a pointer to the variable structure, and a pointer to the next variable in the list.
 */
typedef struct ListNode {
    Variable *variable;
    struct ListNode *next;
} ListNode;

/**
 * @struct List
 *
 * @brief Represents a list of variables.
 */
typedef struct List {
    ListNode *start;
} List;

/**
 * @brief Initializes a List structure.
 *
 * @return The list created.
 */
List *initialize();

/**
 * @brief Inserts a variable at the beginning of the list.
 *
 * @param l Pointer to the List.
 * @param data Pointer to the Variable structure.
 */
void insert(List *l, Variable *data);

/**
 * @brief Clear the entire list.
 *
 * The pointer to the list remains valid.
 *
 * @param l Pointer to the List.
 */
void clean(List *l);

/**
 * @brief Search for a variable in the list.
 *
 * @param l Pointer to the List.
 * @param name Variable name.
 *
 * @return Pointer to the corresponding variable structure, null if not found.
 */
Variable *search(List *l, char *name);

#endif // VARIABLES_H