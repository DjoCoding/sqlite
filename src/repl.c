#define SQLITE_DEBUG
#undef  SQLITE_DEBUG

#define _GNU_SOURCE
#include <stdio.h>

#define BUFFER_SIZE 4 * 1024

#define STRING_VIEW_IMPLEMENTATION
#include <sv.h>

#include <scanner.h>

#include "meta.h"
#include "tokenizer.h"
#include "parser.h"
#include "executor.h"

#include <array.h>

typedef struct {
    Tokenizer   *tokenizer;
    Parser      *parser;
    Executor    *executor;
} Engine;

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
        }
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

int main(void) {
    Scanner *scanner = scanner_create();
    Engine *engine = engine_create();
    
    bool running = true;
    while(running) {
        fprintf(stdout, "sqlite> ");

        char *line = scanner_read(scanner);
        StringView cmd = sv_trim(sv_from_cstr(line));

        if(cmd.len == 0) continue;

        if(sv_starts_with(cmd, sv_from_cstr("."))) {
            MetaCommand meta = metacmd_lookup_command(cmd);
            switch(meta) {
                case META_CMD_EXIT: {
                    running = false;
                    break;
                }
                case META_CMD_UNRECOGNIZED: {
                    fprintf(stdout, "Unrecognized command \'" SV_FMT "\'.\n", SV_ARG(cmd));
                    break;
                }
            }
            continue;
        }

        engine_execute(engine, cmd);
    }


    engine_free(engine);
    scanner_close(scanner);
}