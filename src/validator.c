#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "validator.h"

bool __int__vstring_max(StringView value, usize len) {
    return value.len <= len;
}

bool __int__vinteger_min(int value, int min) {
    return value >= min;
}

bool __int__vinteger_max(int value, int max) {
    return value <= max;
}

Validator validator_init() {
    Validator self = {0};

    self.string.max = __int__vstring_max;

    self.integer.min = __int__vinteger_min;
    self.integer.max = __int__vinteger_max;

    return self;
}


ValidatorResult validator_result_ok() {
    return (ValidatorResult) {.ok=true};
}

ValidatorResult validator_result_error(char *buffer, usize len) {
    char *error = malloc(len + 1);
    if(error == NULL) {
        perror("failed to malloc buffer");
        exit(1);
    }

    memcpy(error, buffer, len);
    return (ValidatorResult) {.ok=false, .as.error=error};
}
