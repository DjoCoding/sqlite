#ifndef TABLE_H_
#define TABLE_H_

#include "types.h"
#include "row.h"
#include "pager.h"

#define ROWS_PER_PAGE           (PAGE_SIZE / sizeof(Row))

typedef struct {
    usize rows_count;
    usize root_page_index;
    usize last_page_index;
    usize pages_count;
    char   _[128];                      // reserved for future metadata
} TableHeader;

typedef struct {
    int     fd;
    usize  rows_count;
    usize  root_page_index;
    usize  last_page_index;
    usize  pages_count;
    Pager  *pager;
} Table;

typedef struct {
    Table *table;
    usize index;
    bool   end;
} TableIterator;

Table *table_open(const char *filename);
void  *table_alloc_row_slot(Table *self);
void   table_close(Table *self);

#endif // TABLE_H_