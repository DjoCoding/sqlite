#include <assert.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h>

#include "pager.h"

Pager *pager_open(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if(fd < 0) {
        perror("failed to open file");
        exit(1);
    }

    off_t len = lseek(fd, 0, SEEK_END);

    Pager *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Pager");
        exit(1);
    }

    self->fd  = fd;
    self->len = len;
    memset(self->pages, 0, sizeof(self->pages));

    return self;
}

static inline size_t __int__pager_get_pages_count(Pager *self) {
    return self->len / PAGE_SIZE;
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

    lseek(self->fd, index * PAGE_SIZE, SEEK_CUR);
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

    lseek(self->fd, 0, SEEK_END);
    write(self->fd, buffer, PAGE_SIZE);
    off_t len = lseek(self->fd, 0, SEEK_END);

    self->len = len;
    self->pages[pages_count] = buffer;

    return self->pages;
}

void pager_flush(Pager *self, size_t index) {
    size_t pages_count = __int__pager_get_pages_count(self);
    assert(index < pages_count);
    assert(self->pages[index] != NULL);

    lseek(self->fd, index * PAGE_SIZE, SEEK_CUR);
    ssize_t len = write(self->fd, self->pages[index], PAGE_SIZE);
    if(len < 0) {
        perror("failed to write page to file");
        exit(1);
    }
}

void pager_close(Pager *self) {
    for(size_t i = 0; i < PAGER_MAX_PAGES; ++i) {
        if(self->pages[i] == NULL) continue;
        pager_flush(self, i);
        free(self->pages[i]);
    }
    close(self->fd);
    free(self);
}