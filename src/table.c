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
        TableHeader header = {.rows_count=0};

        ssize_t bytes_written = write(fd, &header, sizeof(header));
        if(bytes_written < 0) {
            perror("failed to write table file header");
            exit(1);
        }

        self->pager = pager_create(fd, sizeof(TableHeader));
        self->rows_count = 0;

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
    
    size_t data_len = len - sizeof(header);
    size_t estimated_rows_count = data_len / sizeof(Row);

    if(self->rows_count > estimated_rows_count) {
        fprintf(stderr, "Error: invalid table file header.");
        exit(1);
    }

    self->pager = pager_create(fd, sizeof(TableHeader));
    return self;
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

    assert(page_index < PAGER_MAX_PAGES);

    void *page = pager_read(self->pager, page_index);
    return page + offset;
}

void table_close(Table *self) {
    TableHeader header = {
        .rows_count = self->rows_count
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


