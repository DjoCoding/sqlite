#define SQLITE_DEBUG
#undef  SQLITE_DEBUG

#define _GNU_SOURCE
#include <stdio.h>

#define STRING_VIEW_IMPLEMENTATION
#include <sv.h>

#include <array.h>
#include <scanner.h>

#include "meta.h"
#include "engine.h"

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
    
    return 0;
}