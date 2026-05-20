#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"

static inline bool __int__table_has_free_slots(Table *self);

Table *table_open(const char *filename) {
    Table *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Table");
        exit(1);
    }

    self->pager = pager_open(filename);
    self->rows_count = 0;

    return self;
}

static inline bool __int__table_has_free_slots(Table *self) {
    return self->rows_count < TABLE_MAX_ROWS;
}

void *table_alloc_row_slot(Table *self) {
    if(!__int__table_has_free_slots(self)) return NULL;
    void *ptr = table_get_row_slot(self, self->rows_count);
    self->rows_count += 1;
    return ptr;
}

void *table_get_row_slot(Table *self, size_t index) {
    size_t page_index           = index / ROWS_PER_PAGE;
    size_t row_offset_in_page   = index - page_index * ROWS_PER_PAGE;
    size_t offset               = row_offset_in_page * sizeof(Row);

    assert(page_index < PAGER_MAX_PAGES);

    void *page = pager_read(self->pager, page_index);
    return page + offset;
}

void table_close(Table *self) {
    pager_close(self->pager);
    free(self);
}


