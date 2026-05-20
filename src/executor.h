#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include "parser.h"
#include "table.h"
#include "validator.h"

typedef struct {
    Validator   validator;
    Table       *table;
} Executor;

typedef struct {
    bool        ok;
    union {
        char *error;
    } as;
} ExecutorResult;

Executor        *executor_create(Table *table);
ExecutorResult  executor_exec(Executor *self, Statement statement);
void            executor_free(Executor *self);

#endif // EXECUTOR_H_