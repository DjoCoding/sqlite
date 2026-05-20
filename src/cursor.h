#ifndef CURSOR_H_
#define CURSOR_H_

#include "table.h"

typedef struct {
    Table   *table;
    size_t  page_index;
    size_t  cell_index;
    bool    end;    
} Cursor;

Cursor *cursor_create_at_start(Table *table);
Cursor *cursor_create_at_end(Table *table);
void   *cursor_get_value(Cursor *self);
void    cursor_move(Cursor *self);
void    cursor_free(Cursor *self);

#endif // CURSOR_H_