#include <stdlib.h>
#include <time.h>

#define STRING_VIEW_IMPLEMENTATION
#include <sv.h>

#include <engine.h>

void fill_string(char *buffer, usize len, char c) {
    for(usize i = 0; i < len; ++i) {
        buffer[i] = c;
    }
}

int main2(void) {
    fprintf(stdout, "past_username_length: %zu\n", strlen("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    fprintf(stdout, "past_email_length: %zu\n", strlen("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    return 0;
}

int main(void) {
    srand(time(NULL));
    
    Engine *engine = engine_create();

    char long_username[33] = {0};
    fill_string(long_username, sizeof(long_username) - 1, 'a');
    
    char long_email[257] = {0};
    fill_string(long_email, sizeof(long_email) - 1, 'a');

    char buffer[1024] = {0};
    sprintf(buffer, "insert %d \"%s\" \"%s\"", 1, long_username, long_email);
    // fprintf(stdout, "CMD: %s\n", buffer);

    StringView cmd = sv_from_cstr(buffer);
    engine_execute(engine, cmd);

    engine_execute(engine, sv_from_cstr("select"));

    engine_free(engine);
    return 0;
}