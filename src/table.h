#ifndef TABLE_H_
#define TABLE_H_

#include "row.h"
#include "pager.h"

#define ROWS_PER_PAGE           (PAGE_SIZE / sizeof(Row))

typedef struct {
    size_t rows_count;
    size_t root_page_index;
    size_t last_page_index;
    size_t pages_count;
    char   _[128];                      // reserved for future metadata
} TableHeader;

typedef struct {
    int     fd;
    size_t  rows_count;
    size_t  root_page_index;
    size_t  last_page_index;
    size_t  pages_count;
    Pager  *pager;
} Table;

typedef struct {
    Table *table;
    size_t index;
    bool   end;
} TableIterator;

Table *table_open(const char *filename);
void  *table_alloc_row_slot(Table *self);
void  *table_get_row_slot(Table *self, size_t page_index, size_t row_index);
void   table_close(Table *self);

#endif // TABLE_H_