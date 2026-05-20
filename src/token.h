#ifndef TOKEN_H_
#define TOKEN_H_

#include <sv.h>

#include "types.h"

typedef enum {
    TOK_KIND_INVALID = 0,
    TOK_KIND_INSERT,
    TOK_KIND_SELECT,
    TOK_KIND_INT_LIT,
    TOK_KIND_STR_LIT,
} TokenKind;

typedef struct {    
    TokenKind   kind;
    StringView  value;
} Token;

typedef struct {
    Token *items;
    usize len;
    usize size;
} TokenList;

void token_to_string(Token token, char *buffer);

#endif // TOKEN_H_