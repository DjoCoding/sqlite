#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"

static inline bool __int__table_has_free_slots(Table *self);

Table *table_create() {
    Table *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Table");
        exit(1);
    }

    self->rows_count = 0;
    memset(self->pages, 0, sizeof(self->pages));

    return self;
}

void *__int__table_alloc_page(Table *self, size_t page_index) {
    assert(page_index < TABLE_MAX_PAGES);
    assert(self->pages[page_index] == NULL && "must not be allocated");

    self->pages[page_index] = malloc(PAGE_SIZE);
    if(self->pages[page_index] != NULL) return self->pages[page_index];

    perror("failed to malloc Page");
    exit(1);
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

    assert(page_index < TABLE_MAX_PAGES);

    void *page = self->pages[page_index];
    if(page == NULL) {
        page = __int__table_alloc_page(self, page_index);
    }

    return page + offset;
}


void table_free(Table *self) {
    for(size_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        if(self->pages[i] == NULL) continue;
        free(self->pages[i]);
    }
    free(self);
}


