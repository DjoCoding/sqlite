#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"

Executor *executor_create() {
    Executor *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Executor");
        exit(1);
    }

    self->table = table_create();

    return self;
}

ExecutorResult __int__executor_exec_select(Executor *self, SelectStatement select) {
    (void)select;

    for(size_t i = 0; i < self->table->rows_count; ++i) {
        void *bytes = table_get_row_slot(self->table, i);
        Row row = row_from_bytes(bytes);
        fprintf(stdout, "Row(%d, \"%s\", \"%s\")\n", row.id, row.username, row.email);
    }
    
    return (ExecutorResult) {.ok=true};
}

ExecutorResult __int__executor_exec_insert(Executor *self, InsertStatement insert) {
    void *ptr = table_alloc_row_slot(self->table);
    if(ptr == NULL) {
        char buffer[1024] = {0};
        sprintf(buffer, "Error: Table full.");

        char *error = malloc(strlen(buffer) + 1);
        if(error == NULL) return (ExecutorResult) {.ok=false};
        memcpy(error, buffer, strlen(buffer));

        return (ExecutorResult) {
            .ok = false,
            .as.error = error 
        };
    }

    Row row = insert.row;
    row_serialize(row, ptr);

    return (ExecutorResult) {.ok=true};
}


ExecutorResult executor_exec(Executor *self, Statement statement) {
    assert(statement.type != STAT_TYPE_INVALID);

    if(statement.type == STAT_TYPE_INSERT) {
        return __int__executor_exec_insert(self, statement.as.insert);
    }

    if(statement.type == STAT_TYPE_SELECT) {
        return __int__executor_exec_select(self, statement.as.select);
    }

    assert(false && "unreachable");
}


void executor_free(Executor *self) {
    table_free(self->table);
    free(self);
}

