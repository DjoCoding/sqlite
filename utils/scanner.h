#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include "../src/types.h"

typedef struct {
    char    *buffer;
    usize   buffer_length;
    usize   input_length;
} Scanner;

Scanner *scanner_create();
char    *scanner_read(Scanner *self);
void     scanner_close(Scanner *self);

#endif // SCANNER_H_