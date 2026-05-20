#include <stdlib.h>
#include <string.h>

#include "scanner.h"

Scanner *scanner_create() {
    Scanner *self = malloc(sizeof(*self));
    
    if(self == NULL) {
        perror("failed to malloc Scanner");
        exit(1);
    }

    self->buffer = NULL;
    self->buffer_length = 0;
    self->input_length = 0;
    
    return self;
}


char *scanner_read(Scanner *self) {
    if(self->buffer != NULL) {
        free(self->buffer);
        self->buffer = NULL;
    }
    
    isize read = getline(&self->buffer, &self->buffer_length, stdin);
    if(read <= 0) {
        perror("failed to read from stdin");
        exit(1);
    }

    self->input_length = read - 1;
    self->buffer[read - 1] = 0;

    return self->buffer;
}

void scanner_close(Scanner *self) {
    free(self->buffer);
    free(self);
}
