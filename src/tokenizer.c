#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <array.h>

#include "tokenizer.h"

#define TOKEN(k, v)     ((Token) {.kind=k, .value=v})

TokenizerResult __int__tokenizer_result_ok(TokenList tokens) {
    return (TokenizerResult) {
        .ok = true,
        .as.data = tokens
    };
}

TokenizerResult __int__tokenizer_result_error(char *buffer) {
    size_t len = strlen(buffer);
    char *error = calloc(1, sizeof(char) * (len + 1));
    memcpy(error, buffer, len);
    return (TokenizerResult) {
        .ok = false,
        .as.error = error
    };
}


Tokenizer *tokenizer_create() {
    Tokenizer *self = malloc(sizeof(*self));
    self->content = SV_NULL;
    return self;
}

bool __int__tokenizer_end(Tokenizer *self) {
    return self->content.len == 0;
}

StringView __int__tokenizer_split(Tokenizer *self) {
    assert(!__int__tokenizer_end(self));
    self->content = sv_trim(self->content);
    return sv_split(&self->content, ' ');
}

bool __int__tokenizer_is_int(Tokenizer *self, StringView candidate) {
    (void)self;
    
    for(size_t i = 0; i < candidate.len; ++i) {
        if(!isdigit(candidate.data[i])) return false;    
    }
    return true;
}


Token __int__tokenizer_next(Tokenizer *self) {
    assert(!__int__tokenizer_end(self));
    
    StringView next = __int__tokenizer_split(self);
    assert(next.len != 0);

    if(sv_starts_with(next, sv_from_cstr("\""))) {
        bool valid = sv_ends_with(next, sv_from_cstr("\""));
        if(!valid) return TOKEN(TOK_KIND_INVALID, next);
        
        valid = next.len > 1;
        if(!valid) return TOKEN(TOK_KIND_INVALID, next); 

        return TOKEN(TOK_KIND_STR_LIT, sv_substring(next, 1, next.len - 1));
    }

    if(__int__tokenizer_is_int(self, next)) {
        return TOKEN(TOK_KIND_INT_LIT, next);
    }

    if(sv_equal(next, sv_from_cstr("insert"))) return TOKEN(TOK_KIND_INSERT, next);
    if(sv_equal(next, sv_from_cstr("select"))) return TOKEN(TOK_KIND_SELECT, next);
    
    return TOKEN(TOK_KIND_INVALID, next);
}

TokenizerResult tokenizer_tokenize(Tokenizer *self, StringView content) {
    self->content = content;
    TokenList tokens = {0};

    while(!__int__tokenizer_end(self)) {
        Token token = __int__tokenizer_next(self);
        
        if(token.kind == TOK_KIND_INVALID) {
            arrfree(tokens);

            self->content = SV_NULL;

            char buffer[1024] = {0};
            snprintf(buffer, 1024, "Unrecognized keyword '" SV_FMT "'.", SV_ARG(token.value));

            return __int__tokenizer_result_error(buffer);
        }

        arrappend(tokens, token);
    }


    self->content = SV_NULL;
    return __int__tokenizer_result_ok(tokens);
}

void tokenizer_free(Tokenizer *self) {
    free(self);
}

