#include <assert.h>
#include <stdlib.h>
#include <array.h>
#include <string.h>

#include "parser.h"

#define PARSER_ERROR_BUFFER_SIZE 1024

ParserResult __int__parser_result_ok(StatementList list) {
    return (ParserResult) {
        .ok = true,
        .as.data = list
    };
}

ParserResult __int__parser_result_error(char *buffer) {
    size_t len = strlen(buffer);
    char *error = calloc(1, sizeof(char) * (len + 1));
    memcpy(error, buffer, len);

    return (ParserResult) {
        .ok = false,
        .as.error = error
    };
}

Parser *parser_create() {
    Parser *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Parser");
        exit(1);
    }

    self->current = 0;
    self->tokens = (TokenList){0};
    self->error = malloc(sizeof(char) * (PARSER_ERROR_BUFFER_SIZE));
    if(self->error == NULL) {
        perror("failed to malloc buffer");
        exit(1);
    }

    return self;
}

bool __int__parser_end(Parser *self) {
    return self->current >= self->tokens.len;
}

Token __int__parser_peek(Parser *self) {
    assert(!__int__parser_end(self));
    return self->tokens.items[self->current];
}

void __int__parser_consume(Parser *self) {
    self->current += 1;
}

int __int__parser_parse_integer(Parser *self, StringView sv) {
    (void)self;

    int x = 0;
    for(size_t i = 0; i < sv.len; ++i) {
        x *= 10;
        x += sv.data[i] - '0';
    }

    return x;
}

Statement __int__parser_parse_select(Parser *self) {
    assert(__int__parser_peek(self).kind == TOK_KIND_SELECT);
    __int__parser_consume(self);

    
    if(!__int__parser_end(self)) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected end but found more.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    return (Statement) {
        .type = STAT_TYPE_SELECT
    };
}

Statement __int__parser_parse_insert(Parser *self) {
    assert(__int__parser_peek(self).kind == TOK_KIND_INSERT);
    __int__parser_consume(self);

    if(__int__parser_end(self)) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected id argument but end was found.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    if(__int__parser_peek(self).kind != TOK_KIND_INT_LIT) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected id argument but else was found.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    int id = __int__parser_parse_integer(self, __int__parser_peek(self).value);
    __int__parser_consume(self);

    if(__int__parser_end(self)) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected username argument but end was found.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    if(__int__parser_peek(self).kind != TOK_KIND_STR_LIT) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected username argument but else was found.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    StringView username = __int__parser_peek(self).value;
    __int__parser_consume(self);

    if(__int__parser_end(self)) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected email argument but end was found.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    if(__int__parser_peek(self).kind != TOK_KIND_STR_LIT) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected email argument but else was found.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    StringView email = __int__parser_peek(self).value;
    __int__parser_consume(self);

    if(!__int__parser_end(self)) {
        snprintf(self->error, PARSER_ERROR_BUFFER_SIZE, "expected end but found more.");
        return (Statement){.type=STAT_TYPE_INVALID};
    }

    Row row = row_init(id, username, email);
    return (Statement){
        .type = STAT_TYPE_INSERT,
        .as.insert = {
            .row = row
        }
    };
}

Statement __int__parser_next(Parser *self) {
    Token token = __int__parser_peek(self);

    switch (token.kind) {
        case TOK_KIND_INSERT:
            return __int__parser_parse_insert(self);
        case TOK_KIND_SELECT:
            return __int__parser_parse_select(self);
        default:
            sprintf(self->error, "invalid statement starting with '" SV_FMT "'.", SV_ARG(token.value));
            return (Statement){.type=STAT_TYPE_INVALID};
    }

    return (Statement){.type=STAT_TYPE_INVALID};
}

ParserResult parser_parse(Parser *self, TokenList tokens) {
    self->current = 0;
    self->tokens = tokens;

    StatementList list = {0};
    memset(self->error, 0, PARSER_ERROR_BUFFER_SIZE);

    while(!__int__parser_end(self)) {
        Statement statement = __int__parser_next(self);
        if(statement.type == STAT_TYPE_INVALID) {
            arrfree(list);
            return __int__parser_result_error(self->error);
        }

        arrappend(list, statement);
    }

    self->current = 0;
    self->tokens = (TokenList){0};

    return __int__parser_result_ok(list);
}

void parser_free(Parser *self) {
    free(self->error);
    free(self);
}

static char *STATEMENT_KIND_TO_STRING_MAP[] = {
    [STAT_TYPE_INVALID] = "STAT_TYPE_INVALID",
    [STAT_TYPE_INSERT] = "STAT_TYPE_INSERT",
};

void statement_to_string(Statement self, char *buffer) {
    sprintf(buffer, "Statement(type=%s)", STATEMENT_KIND_TO_STRING_MAP[self.type]);
}