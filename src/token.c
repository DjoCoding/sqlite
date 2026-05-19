#include "token.h"

static char *TOKEN_KIND_TO_STRING_MAP[] = {
    [TOK_KIND_INVALID] =  "TOK_KIND_INVALID",
    [TOK_KIND_INSERT]  =  "TOK_KIND_INSERT",
    [TOK_KIND_INT_LIT] =  "TOK_KIND_INT_LIT",
    [TOK_KIND_STR_LIT] =  "TOK_KIND_STR_LIT",
};

void token_to_string(Token token, char *buffer) {
    char *kind = TOKEN_KIND_TO_STRING_MAP[token.kind];
    sprintf(buffer, "Token(kind=%s, .value='" SV_FMT "')", kind, SV_ARG(token.value));
}