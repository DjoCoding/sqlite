#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include "parser.h"
#include "table.h"


typedef struct {
    char _;
    Table *table;
} Executor;

typedef struct {
    bool        ok;
    union {
        char *error;
    } as;
} ExecutorResult;

Executor        *executor_create();
ExecutorResult  executor_exec(Executor *self, Statement statement);
void            executor_free(Executor *self);

#endif // EXECUTOR_H_