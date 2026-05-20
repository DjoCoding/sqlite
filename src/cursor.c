#include <stdlib.h>

#include "cursor.h"

Cursor *__int__cursor_create_raw(Table *table) {
    Cursor *self = malloc(sizeof(*self));
    self->table = table;
    return self;
}

Cursor *cursor_create_at_start(Table *table) {
    Cursor *self = __int__cursor_create_raw(table);
    self->index = 0;
    self->end = table->rows_count == 0;
    return self;
}

Cursor *cursor_create_at_end(Table *table) {
    Cursor *self = __int__cursor_create_raw(table);
    self->index  = table->rows_count;
    self->end = true;
    return self;
}

void *cursor_get_value(Cursor *self) {
    if(self->end) return table_alloc_row_slot(self->table);
    return table_get_row_slot(self->table, self->index);
}

void cursor_move(Cursor *self) {
    if(self->end) return;

    self->index += 1;
    if(self->index == self->table->rows_count) {
        self->end = true;
    }
}

void cursor_free(Cursor *self) {
    free(self);
}