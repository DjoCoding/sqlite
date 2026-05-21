#include <assert.h>
#include <stdlib.h>

#include "node.h"
#include "cursor.h"

Cursor *__int__cursor_create_raw(Table *table) {
    Cursor *self = malloc(sizeof(*self));
    self->table = table;
    return self;
}

Cursor *cursor_create_at_start(Table *table) {
    Cursor *self = __int__cursor_create_raw(table);
    self->page_index = self->table->root_page_index;
    self->cell_index = 0;
    self->end = self->table->rows_count == 0;
    return self;
}

Cursor *cursor_create_at_end(Table *table) {
    Cursor *self = __int__cursor_create_raw(table);
    self->page_index = self->table->last_page_index;
    self->cell_index = self->table->rows_count % ROWS_PER_PAGE;
    self->end = true;
    return self;
}

void *cursor_get_value(Cursor *self) {
    if(self->end) return table_alloc_row_slot(self->table);

    Page page = pager_read(self->table->pager, self->page_index);
    NodeHeader header = node_header_deserialize(page.ptr);

    usize offset = LEAF_NODE_HEADER_SIZE + self->cell_index * LEAF_NODE_CELL_SIZE;
    if(header.kind == NODE_KIND_INTERNAL) {
        assert(false && "make sure to add support to internal nodes.");
    }

    return page.ptr + offset;
}

void cursor_move(Cursor *self) {
    if(self->end) return;

    assert(self->page_index <= self->table->last_page_index);
    
    if(self->page_index < self->table->last_page_index) {
        self->cell_index += 1;
        
        if(self->cell_index % ROWS_PER_PAGE != 0) return;
        
        self->cell_index  = 0;
        self->page_index += 1;
        
        return;
    }

    assert(self->cell_index < (self->table->rows_count % ROWS_PER_PAGE));
    self->cell_index += 1;

    self->end = self->cell_index == (self->table->rows_count % ROWS_PER_PAGE);
}

void cursor_free(Cursor *self) {
    free(self);
}