#define NOB_IMPLEMENTATION
#include "nob.h"


#define SRC_DIR   "src"
#define UTILS_DIR "utils"
#define TESTS_DIR "tests"

#define CFLAGS "-Wall", "-Wextra", "-Werror", "-ggdb2"

typedef struct {
    Cmd *cmd;
    bool include_repl;
} Source_Context;

void flags(Cmd *cmd) {
    nob_cmd_append(cmd, CFLAGS);
}

void include(Cmd *cmd, bool include_src) {
    nob_cmd_append(cmd, "-I./clib", "-I./utils");
    if(include_src) nob_cmd_append(cmd, "-I./src");
}

void add_source_file(Cmd *cmd, const char *path) {
    nob_cmd_append(cmd, path);
}

bool walk_source_dir(Nob_Walk_Entry entry) {
    if (entry.type != FILE_REGULAR) {
        return true;
    }

    Source_Context *ctx = entry.data;

    String_View path = sv_from_cstr(entry.path);

    if (!sv_ends_with(path, sv_from_cstr(".c"))) {
        return true;
    }

    if (!ctx->include_repl &&
        sv_ends_with(path, sv_from_cstr("repl.c"))) {
        return true;
    }

    nob_cmd_append(ctx->cmd, strdup(entry.path));

    return true;
}

void add_project_sources(Cmd *cmd, bool include_repl) {
    Source_Context ctx = {
        .cmd = cmd,
        .include_repl = include_repl,
    };

    nob_walk_dir(SRC_DIR, walk_source_dir, .data = &ctx);
    nob_walk_dir(UTILS_DIR, walk_source_dir, .data = &ctx);
}

void build_main_binary(void) {
    Cmd cmd = {0};

    nob_cmd_append(&cmd, "cc");
    flags(&cmd);
    include(&cmd, false);

    nob_cmd_append(&cmd, "-o", "build/sqlite");

    add_project_sources(&cmd, true);

    nob_cmd_run_sync(cmd);
}

void build_test(const char *name) {
    Cmd cmd = {0};

    char test_path[256];
    char output_path[256];

    snprintf(test_path, sizeof(test_path),
        TESTS_DIR"/%s.c", name);

    snprintf(output_path, sizeof(output_path),
        "build/%s", name);

    nob_cmd_append(&cmd, "cc");
    flags(&cmd);
    include(&cmd, true);

    nob_cmd_append(&cmd, "-o", output_path);

    add_project_sources(&cmd, false);
    nob_cmd_append(&cmd, test_path);

    nob_cmd_run_sync(cmd);
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    mkdir_if_not_exists("build");

    if (argc >= 3 && strcmp(argv[1], "test") == 0) {
        build_test(argv[2]);
        return 0;
    }

    build_main_binary();

    return 0;
}