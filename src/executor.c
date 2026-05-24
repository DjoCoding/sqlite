#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cursor.h"
#include "node.h"
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
        void *bytes    = cursor_get_value(cursor);
        LeafCell *cell = leaf_cell_deserialize(bytes);
        
        void *raw     = cell->raw;
        Row   row     = row_from_bytes(raw);

        fprintf(stdout, "Row(%d, \"%.*s\", \"%.*s\")\n", row.id, ROW_USERNAME_SIZE, row.username, ROW_EMAIL_SIZE, row.email);
        
        leaf_cell_free(cell);
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

    Row row = row_init_from_raw(raw_row);

    Page  root_page = pager_read(self->table->pager, self->table->root_page_index); 
    Node  root      = node_deserialize(root_page);

    Node node = root;
    while(node.kind != NODE_KIND_LEAF) {
        // make the traversal
    }

    Page     leaf_page = pager_read(self->table->pager, node.page_id);
    LeafNode *leaf     = leaf_node_deserialize(leaf_page.ptr);
    int      result    = leaf_node_insert(leaf, row.id, row);

    if(result == LEAF_NODE_INSERT_RESULT_KEY_EXISTS) {
        leaf_node_free(leaf);

        char buffer[1024] = {0};
        sprintf(buffer, "Error: key %d already exists in table.", row.id);

        usize buflen = strlen(buffer);
        
        char *error = malloc(buflen + 1);
        if(error == NULL) return (ExecutorResult) {.ok=false};
        
        memcpy(error, buffer, buflen);
        error[strlen(buffer)] = 0;

        return (ExecutorResult) {
            .ok = false,
            .as.error = error 
        };
    }

    leaf_node_serialize(leaf, leaf_page.ptr);
    self->table->rows_count += 1;

    assert(result == LEAF_NODE_INSERT_RESULT_SUCCESS);
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

