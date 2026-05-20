#ifndef ENGINE_H_
#define ENGINE_H_

#include "tokenizer.h"
#include "parser.h"
#include "executor.h"

typedef struct {
    Tokenizer   *tokenizer;
    Parser      *parser;
    Executor    *executor;
} Engine;

Engine *engine_create(Table *table);
bool    engine_execute(Engine *self, StringView cmd);
void    engine_free(Engine *self);

#endif // ENGINE_H_