#include "types.h"

Variable *create_var(char *name, Types type, int size) {
    Variable *v = (Variable *)malloc(sizeof(Variable));
    if (!v) return NULL;

    v->name = strdup(name);
    v->type = type;
    v->initialized = 0;
    v->size = size;
    v->data = NULL;

    return v;
}
