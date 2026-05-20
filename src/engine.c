#include <array.h>

#include "engine.h"

Engine *engine_create() {
    Engine *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc Engine");
        exit(1);
    }

    self->tokenizer = tokenizer_create();
    self->parser    = parser_create();
    self->executor  = executor_create();

    return self;
}

bool engine_execute(Engine *self, StringView cmd) {
    #ifdef SQLITE_DEBUG
    char buffer[1024] = {0};
    #endif

    TokenizerResult tokenizer_result = tokenizer_tokenize(self->tokenizer, cmd); 
    if(!tokenizer_result.ok) {
        char *error = tokenizer_result.as.error;
        fprintf(stderr, "%s\n", error);
        free(error);
        return false;
    }

    TokenList tokens = tokenizer_result.as.data;

    #ifdef SQLITE_DEBUG
    memset(buffer, 0, sizeof(buffer));
    fprintf(stdout, "TOKENS\n");
    arrforeach(tokens, _, item, {
        token_to_string(item, buffer);
        fprintf(stdout, "%s\n", buffer);
    })
    #endif // SQLITE_DEBUG


    ParserResult parser_result = parser_parse(self->parser, tokens);
    if(!parser_result.ok) {
        arrfree(tokens);
        char *error = parser_result.as.error;
        fprintf(stderr, "%s\n", error);
        free(error);
        return false;
    }

    StatementList statements = parser_result.as.data;

    #ifdef SQLITE_DEBUG
    memset(buffer, 0, sizeof(buffer));
    fprintf(stdout, "STATEMENTS\n");
    arrforeach(statements, _, item, {
        statement_to_string(item, buffer);
        fprintf(stdout, "%s\n", buffer);
    })
    #endif // SQLITE_DEBUG

    arrforeach(statements, _, statement, {
        ExecutorResult executor_result = executor_exec(self->executor, statement);
        if(!executor_result.ok) {
            char *error = executor_result.as.error;
            fprintf(stderr, "%s\n", error);
            free(error);
            continue;
        }
        fprintf(stdout, "Executed.\n");
    })

    arrfree(statements);
    arrfree(tokens);

    return true;
}

void engine_free(Engine *self) {
    tokenizer_free(self->tokenizer);
    parser_free(self->parser);
    executor_free(self->executor);
    free(self);
}