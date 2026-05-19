#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <stdio.h>
#include <sv.h>

#include "token.h"

typedef struct {
    bool ok;
    union {
        char        *error;
        TokenList   data;
    } as;
} TokenizerResult;

typedef struct {
    StringView  content;
} Tokenizer;

Tokenizer       *tokenizer_create();
TokenizerResult  tokenizer_tokenize(Tokenizer *self, StringView content);
void             tokenizer_free(Tokenizer *self);


#endif // TOKENIZER_H_