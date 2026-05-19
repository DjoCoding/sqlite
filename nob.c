#define NOB_IMPLEMENTATION
#include "nob.h"

#define SRC_DIR "src"
#define UTILS_DIR "utils"
#define CFLAGS "-Wall", "-Wextra", "-Werror", "-ggdb2"

void flags(Cmd *cmd) {
    nob_cmd_append(cmd, CFLAGS);
}

void include(Cmd *cmd) {
    nob_cmd_append(cmd, "-I./clib", "-I./utils");
}

void add_source_file(Cmd *cmd, const char *path) {
    nob_cmd_append(cmd, path);
}

typedef bool (*Nob_Walk_Func)(Nob_Walk_Entry entry);

bool walk_source_dir(Nob_Walk_Entry entry) {
    if (entry.type == FILE_REGULAR) {
        String_View sv = sv_from_cstr(entry.path);

        if (sv_ends_with(sv, sv_from_cstr(".c"))) {
            Cmd *cmd = (Cmd *)entry.data;
            char *copy = strdup(entry.path);
            add_source_file(cmd, copy);
        }
    }

    return true;
}

void source(Cmd *cmd) {
    char *src = SRC_DIR;
    nob_walk_dir(src, walk_source_dir, .data=cmd);

    char *utils = UTILS_DIR;
    nob_walk_dir(utils, walk_source_dir, .data=cmd);
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    
    mkdir_if_not_exists("build");

    Cmd cmd = {0};
    nob_cmd_append(&cmd, "cc", "-o", "build/sqlite");
    flags(&cmd);
    include(&cmd);
    source(&cmd);
    
    nob_cmd_run(&cmd);

    return 0;
}