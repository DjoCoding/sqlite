#ifndef PAGER_H_
#define PAGER_H_

#include <stdio.h>

#define PAGER_MAX_PAGES         200
#define PAGE_SIZE               (4 * 1024) 

typedef struct {
    int     fd;
    usize  skip;
    usize  pages_count;
    void   *pages[PAGER_MAX_PAGES];
} Pager;

typedef struct { 
    usize  id;
    void  *ptr;
} Page;

Pager *pager_create(int fd, usize skip);
Page   pager_read(Pager *self, usize index);
Page   pager_alloc(Pager *self);
void   pager_flush(Pager *self, usize index);
void   pager_free(Pager *self);

#endif // PAGER_H_