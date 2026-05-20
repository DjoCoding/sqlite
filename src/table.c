#include <assert.h>
#include <fcntl.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

#include "table.h"

static inline bool __int__table_has_free_slots(Table *self);

Table *table_open(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if(fd < 0) {
        perror("failed to open table file");
        exit(1);
    }

    ssize_t len = lseek(fd, 0, SEEK_END);
    if(len < 0) {
        perror("failed to seek to table file");
        exit(1);
    }

    Table *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Table");
        exit(1);
    }

    self->fd = fd;

    if(len == 0) {
        TableHeader header = {.rows_count=0, .root_page_index=0, .last_page_index=0, .pages_count=0};

        ssize_t bytes_written = write(fd, &header, sizeof(header));
        if(bytes_written < 0) {
            perror("failed to write table file header");
            exit(1);
        }

        self->pager = pager_create(fd, sizeof(TableHeader));
        self->rows_count = 0;
        self->root_page_index = 0;
        self->last_page_index = 0;
        self->pages_count = 0;

        return self;
    }

    if((size_t)len < sizeof(TableHeader)) {
        fprintf(stderr, "Error: corrupted table file.");
        exit(1);
    }

    if(lseek(fd, 0, SEEK_SET) < 0) {
        perror("failed to seek in table file");
        exit(1);
    }
    
    TableHeader header = {0};

    ssize_t bytes_read = read(fd, &header, sizeof(header));
    if(bytes_read < 0) {
        perror("failed to read table file header");
        exit(1);
    }

    self->rows_count = header.rows_count;
    self->root_page_index = header.root_page_index;
    self->last_page_index = header.last_page_index;
    self->pages_count = header.pages_count;
    
    size_t max_rows_count = self->pages_count * ROWS_PER_PAGE;

    if(self->rows_count > max_rows_count) {
        fprintf(stderr, "Error: invalid table file header.");
        exit(1);
    }

    self->pager = pager_create(fd, sizeof(TableHeader));
    return self;
}

static inline bool __int__table_has_free_slots(Table *self) {
    return (self->pages_count * ROWS_PER_PAGE - self->rows_count) > 0;
}

void *table_alloc_row_slot(Table *self) {
    if(__int__table_has_free_slots(self)) {
        void *slot = table_get_row_slot(self, self->last_page_index, self->rows_count % ROWS_PER_PAGE); 
        self->rows_count += 1;
        return slot;
    }
    
    void *page = pager_alloc(self->pager);
    self->rows_count  += 1;
    self->pages_count += 1;
    self->last_page_index = self->pager->pages_count - 1;

    return page;
}

void *table_get_row_slot(Table *self, size_t page_index, size_t row_index) {
    assert(page_index < PAGER_MAX_PAGES);
    size_t offset = row_index * sizeof(Row);
    void *page = pager_read(self->pager, page_index);
    return page + offset;
}

void table_close(Table *self) {
    TableHeader header = {
        .rows_count = self->rows_count,
        .root_page_index = self->root_page_index,
        .last_page_index = self->last_page_index,
        .pages_count = self->pages_count
    };
    
    if(lseek(self->fd, 0, SEEK_SET) < 0) {
        perror("failed to seek in table file");
        exit(1);
    }
    
    ssize_t bytes_written = write(self->fd, &header, sizeof(header));
    if(bytes_written < 0) {
        perror("failed to write table file header");
        exit(1);
    }
    
    pager_free(self->pager);
    close(self->fd);
    free(self);
}


