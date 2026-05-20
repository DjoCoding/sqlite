#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cursor.h"

#include "executor.h"

Executor *executor_create(Table *table) {
    Executor *self = malloc(sizeof(*self));
    if(self == NULL) { 
        perror("failed to malloc Executor");
        exit(1);
    }

    self->validator = validator_init();
    self->table     = table;

    return self;
}

ExecutorResult __int__executor_exec_select(Executor *self, SelectStatement select) {
    (void)select;

    Cursor *cursor = cursor_create_at_start(self->table);
    while(!cursor->end) {
        void *bytes = cursor_get_value(cursor);
        Row row = row_from_bytes(bytes);
        fprintf(stdout, "Row(%d, \"%.*s\", \"%.*s\")\n", row.id, ROW_USERNAME_SIZE, row.username, ROW_EMAIL_SIZE, row.email);
        cursor_move(cursor);
    }
    cursor_free(cursor);
    
    return (ExecutorResult) {.ok=true};
}

ValidatorResult __int__executor_validate(Executor *self, RawRow row) {
    bool isvalid = true;
    
    isvalid = self->validator.integer.min(row.id, 1);
    if(!isvalid) {
        char buffer[1024] = {0};
        sprintf(buffer, "Error: invalid id value id=%d, expected to be > 1.", row.id);
        return validator_result_error(buffer, strlen(buffer));        
    }

    isvalid = self->validator.integer.max(row.id, 4);
    if(!isvalid) {
        char buffer[1024] = {0};
        sprintf(buffer, "Error: invalid id value id=%d, expected to be <= 4.", row.id);
        return validator_result_error(buffer, strlen(buffer));        
    }

    isvalid = self->validator.string.max(row.username, ROW_USERNAME_SIZE);
    if(!isvalid) {
        char buffer[1024] = {0};
        sprintf(buffer, "Error: string too long, expected len(username) <= %d", ROW_USERNAME_SIZE);
        return validator_result_error(buffer, strlen(buffer));        
    }

    isvalid = self->validator.string.max(row.email, ROW_EMAIL_SIZE);
    if(!isvalid) {
        char buffer[1024] = {0};
        sprintf(buffer, "Error: string too long, expected len(email) <= %d", ROW_EMAIL_SIZE);
        return validator_result_error(buffer, strlen(buffer));        
    }

    return (ValidatorResult){.ok=true};

}

ExecutorResult __int__executor_exec_insert(Executor *self, InsertStatement insert) {
    RawRow raw_row = insert.row;

    ValidatorResult vresult = __int__executor_validate(self, raw_row);
    if(!vresult.ok) {
        return (ExecutorResult){.ok=false, .as.error=vresult.as.error};
    }

    Cursor *cursor = cursor_create_at_end(self->table);
    void *slot     = cursor_get_value(cursor);
    cursor_free(cursor);

    if(slot == NULL) {
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

    Row row = row_init_from_raw(raw_row);
    row_serialize(row, slot);

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
    free(self);
}

