#include <assert.h>
#include <stdlib.h>

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
    return table_get_row_slot(self->table, self->page_index, self->cell_index);
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