#ifndef PAGER_H_
#define PAGER_H_

#include <stdio.h>

#define PAGER_MAX_PAGES         100
#define PAGE_SIZE               (4 * 1024) 

typedef struct {
    int     fd;
    size_t  len;
    void   *pages[PAGER_MAX_PAGES];
} Pager;

Pager *pager_open(const char *filename);
void  *pager_read(Pager *self, size_t index);
void  *pager_alloc(Pager *self);
void   pager_flush(Pager *self, size_t index);
void   pager_close(Pager *self);

#endif // PAGER_H_