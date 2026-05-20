#include <assert.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h>

#include "types.h"
#include "pager.h"

/**
 * Pager: responsible of caching pages and allocating new pages on demand
 * pager_alloc: allocate a new page at the end of the file
 * pager_read:  read an existing page within the file
 */
 
// skip: header size skipped
Pager *pager_create(int fd, usize skip) {
    isize len = lseek(fd, 0, SEEK_END);
    if(len < 0) {
        perror("failed to seek in pages file");
        exit(1);
    }

    assert(skip <= (usize)len);

    Pager *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Pager");
        exit(1);
    }

    memset(self->pages, 0, sizeof(self->pages));
    
    self->fd    = fd;
    self->skip  = skip;

    self->pages_count = (len - skip) / PAGE_SIZE;
    if(((usize)len - skip) % PAGE_SIZE != 0) {
        fprintf(stderr, "Error: corrupted table file.");
        exit(1);
    }

    return self;
}

static inline off_t __int__pager_seek_start(Pager *self, usize offset) {
    return lseek(self->fd, offset + self->skip, SEEK_SET);
}

static inline off_t __int__pager_seek_end(Pager *self, usize offset) {
    return lseek(self->fd, offset, SEEK_END);
}

void *pager_read(Pager *self, usize index) {
    assert(index < self->pages_count);
    
    if(self->pages[index] != NULL) return self->pages[index];
    
    void *buffer = malloc(PAGE_SIZE);
    if(buffer == NULL) {
        perror("failed to malloc buffer");
        exit(1);
    }

    __int__pager_seek_start(self, index * PAGE_SIZE);
    isize len = read(self->fd, buffer, PAGE_SIZE);
    if(len < 0) {
        perror("failed to read page from file");
        exit(1);
    }

    self->pages[index] = buffer;
    return self->pages[index];
}

void *pager_alloc(Pager *self) {
    assert(self->pages_count < PAGER_MAX_PAGES);

    void *buffer = malloc(PAGE_SIZE);
    if(buffer == NULL) {
        perror("failed to malloc page buffer");
        exit(1);
    }
    memset(buffer, 0, PAGE_SIZE);

    __int__pager_seek_end(self, 0);
    write(self->fd, buffer, PAGE_SIZE);
    
    self->pages[self->pages_count] = buffer;
    self->pages_count += 1;

    return self->pages[self->pages_count - 1];
}

void pager_flush(Pager *self, usize index) {
    assert(index < self->pages_count);
    assert(self->pages[index] != NULL);

    __int__pager_seek_start(self, index * PAGE_SIZE);
    isize len = write(self->fd, self->pages[index], PAGE_SIZE);
    if(len < 0) {
        perror("failed to write page to file");
        exit(1);
    }
}

void pager_free(Pager *self) {
    for(usize i = 0; i < self->pages_count; ++i) {
        if(self->pages[i] == NULL) continue;
        pager_flush(self, i);
        free(self->pages[i]);
    }
    free(self);
}