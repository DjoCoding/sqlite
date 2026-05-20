#define STRING_VIEW_IMPLEMENTATION
#include <sv.h>

#include <engine.h>

int main(void) {
    Engine *engine = engine_create();

    for(int i = 0; i < 1401; ++i) {
        char buffer[1024] = {0};
        sprintf(buffer, "insert %d \"user%d\" \"user%d@gmail.com\"", i, i, i);

        StringView cmd = sv_from_cstr(buffer);
        engine_execute(engine, cmd);
    }

    engine_free(engine);
    return 0;
}