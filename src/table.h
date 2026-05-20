#ifndef TABLE_H_
#define TABLE_H_

#include "row.h"

#define TABLE_MAX_PAGES         100
#define PAGE_SIZE               (4 * 1024) 
#define ROWS_PER_PAGE           (PAGE_SIZE / sizeof(Row))
#define TABLE_MAX_ROWS          (ROWS_PER_PAGE * TABLE_MAX_PAGES)

typedef struct {
    size_t  rows_count;
    void   *pages[TABLE_MAX_PAGES];
} Table;

Table *table_create();
void  *table_alloc_row_slot(Table *self);
void  *table_get_row_slot(Table *self, size_t index);
void   table_free(Table *self);

#endif // TABLE_H_