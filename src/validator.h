#ifndef VALIDATOR_H_
#define VALIDATOR_H_

#include <sv.h>

typedef struct {
    bool        ok;
    union {
        char   *error;
    } as;
} ValidatorResult;

typedef struct {
    struct {
        bool (*max)(StringView value, usize len);
    } string;


    struct {
        bool (*min)(int value, int min);
        bool (*max)(int value, int min);
    } integer;
} Validator;

Validator       validator_init();
ValidatorResult validator_result_ok();
ValidatorResult validator_result_error(char *buffer, usize len);

#endif // VALIDATOR_H_