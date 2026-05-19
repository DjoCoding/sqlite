#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>

typedef struct {
    char    *buffer;
    size_t   buffer_length;
    size_t   input_length;
} Scanner;

Scanner *scanner_create();
char    *scanner_read(Scanner *self);
void     scanner_close(Scanner *self);

#endif // SCANNER_H_