#ifndef PARSER_H_
#define PARSER_H_

#include "tokenizer.h"
#include "statement.h"
#include "table.h"

typedef struct {
    TokenList tokens;
    usize    current;
    char     *error;
} Parser;

typedef struct {
    bool ok;
    union {
        char *error;
        StatementList data;
    } as;
} ParserResult;

Parser         *parser_create();
ParserResult    parser_parse(Parser *self, TokenList tokens);
void            parser_free(Parser *parser);

void statement_to_string(Statement self, char *buffer);

#endif
