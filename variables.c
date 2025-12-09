#include <stdlib.h>
#include "variables.h"

List *initialize() {
    List *l = (List *)malloc(sizeof(List));
    if (!l) return NULL;

    l->start = NULL;
    return l;
}

void insert(List *l, Variable *data) {
    if (!l) return;

    ListNode *n = (ListNode *)malloc(sizeof(ListNode));
    if (!n) return;

    n->variable = data;
    n->next = l->start;
    l->start = n;
}

Variable *search(List *l, char *name) {
    if (!l) return NULL;

    ListNode *n = l->start;
    while (n && strcmp(n->variable->name, name) != 0) {
        n = n->next;
    }

    if (!n) return NULL;
    return n->variable;
}

void clean(List *l) {
    if (!l) return;

    while (l->start) {
        ListNode *n = l->start;

        if (n->variable) {
            free(n->variable->name);
            if (n->variable->initialized == 1) {
                free(n->variable->data);
            }
            free(n->variable);
        }

        l->start = n->next;
        free(n);
    }
}