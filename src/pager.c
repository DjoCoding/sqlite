#include <assert.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h>

#include "pager.h"

// skip: header size skipped
Pager *pager_create(int fd, size_t skip) {
    ssize_t len = lseek(fd, 0, SEEK_END);
    if(len < 0) {
        perror("failed to seek in pages file");
        exit(1);
    }

    assert(skip <= (size_t)len);

    Pager *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Pager");
        exit(1);
    }

    self->fd    = fd;
    self->skip  = skip;
    self->len   = len - skip;
    memset(self->pages, 0, sizeof(self->pages));

    return self;
}

static inline size_t __int__pager_get_pages_count(Pager *self) {
    return self->len / PAGE_SIZE;
}

static inline off_t __int__pager_seek_start(Pager *self, size_t offset) {
    return lseek(self->fd, offset + self->skip, SEEK_SET);
}

static inline off_t __int__pager_seek_end(Pager *self, size_t offset) {
    return lseek(self->fd, offset, SEEK_END);
}

void *pager_read(Pager *self, size_t index) {
    size_t pages_count = __int__pager_get_pages_count(self);
    assert(index <= pages_count);

    if(index == pages_count) return pager_alloc(self);
    
    if(self->pages[index] != NULL) return self->pages[index];
    
    void *buffer = malloc(PAGE_SIZE);
    if(buffer == NULL) {
        perror("failed to malloc buffer");
        exit(1);
    }

    __int__pager_seek_start(self, index * PAGE_SIZE);
    ssize_t len = read(self->fd, buffer, PAGE_SIZE);
    if(len < 0) {
        perror("failed to read page from file");
        exit(1);
    }

    self->pages[index] = buffer;
    return self->pages[index];
}

void *pager_alloc(Pager *self) {
    size_t pages_count = __int__pager_get_pages_count(self);
    if(pages_count >= PAGER_MAX_PAGES) return NULL;
    
    void *buffer = malloc(PAGE_SIZE);
    if(buffer == NULL) {
        perror("failed to malloc page buffer");
        exit(1);
    }
    memset(buffer, 0, PAGE_SIZE);

    __int__pager_seek_end(self, 0);
    write(self->fd, buffer, PAGE_SIZE);
    off_t len = __int__pager_seek_end(self, 0);

    self->len = len - self->skip;
    self->pages[pages_count] = buffer;

    return self->pages[pages_count];
}

void pager_flush(Pager *self, size_t index) {
    size_t pages_count = __int__pager_get_pages_count(self);
    assert(index < pages_count);
    assert(self->pages[index] != NULL);

    __int__pager_seek_start(self, index * PAGE_SIZE);
    ssize_t len = write(self->fd, self->pages[index], PAGE_SIZE);
    if(len < 0) {
        perror("failed to write page to file");
        exit(1);
    }
}

void pager_free(Pager *self) {
    for(size_t i = 0; i < PAGER_MAX_PAGES; ++i) {
        if(self->pages[i] == NULL) continue;
        pager_flush(self, i);
        free(self->pages[i]);
    }
    free(self);
}