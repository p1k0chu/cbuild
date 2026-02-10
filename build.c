#include "build.h"

#include <cbuild.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

const char *srcs[] = {
    "compile.c",
    "compilemyself.c",
    "filename.c",
    "mtime.c",
    "object.c",
    "target.c",
    "utils.c",
    "custom_target.c",
    "pkgconfig.c",
};

#define COMMON_CFLAGS                                                                 \
    "-Wall", "-Wextra", "-iquote", "./include", "-std=gnu23", "-Werror=implicit-int", \
        "-Wconversion", "-Werror=implicit-fallthrough"

[[noreturn]] static void print_help();

char *prog;

int main(int argc, char **argv) {
    char willcompileself = 1;
    int flags = 0;
    prog = argv[0];
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0)
            willcompileself = 0;
        else if (strcmp(argv[i], "-S") == 0)
            willcompileself |= 2;
        else if (strcmp(argv[i], "-n") == 0)
            flags |= CBUILD_COMPILE_DRYRUN;
        else if (strcmp(argv[i], "-B") == 0)
            flags |= CBUILD_COMPILE_FORCE;
        else if (strcmp(argv[i], "-h") == 0)
            print_help();
    }

    if (willcompileself) {
        cbuild_recompile_myself(__FILE__,
                                argv,
                                flags,
                                "-Wall",
                                "-Wextra",
                                CBUILD_SELFCOMPILE_FLAGS,
                                NULL);
        if (willcompileself > 1)
            return 0;
    }

    cbuild_staticlib_t *liba = cbuild_create_staticlib("libcbuild.a", NULL);
    cbuild_sharedlib_t *libso = cbuild_create_sharedlib("libcbuild.so", NULL);

    const size_t n = sizeof(srcs) / sizeof(*srcs);
    cbuild_obj_t *objs[n];
    for (size_t i = 0; i < n; ++i) {
        objs[i] = cbuild_obj_create(srcs[i], COMMON_CFLAGS, NULL);
    }

    cbuild_target_append_deps((void *)liba, (void *)objs, n);
    cbuild_target_append_deps((void *)libso, (void *)objs, n);

    int ws;
    int status = 0;
    pid_t cpid = cbuild_target_compile((void *)liba, flags);
    if (cpid > 0) {
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        status = WEXITSTATUS(ws);
    }

    cpid = cbuild_target_compile((void *)libso, flags);
    if (cpid > 0) {
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        status |= WEXITSTATUS(ws);
    }

    return status;
}

[[noreturn]] static void print_help() {
    printf(
        "Usage: %s [OPTIONS]\n\n"
        "Options:\n"
        "-n - dry run\n"
        "-B - force recompile\n"
        "-s - do not recompile myself\n"
        "-S - only recompile myself\n"
        "-h - show help",
        prog);
    exit(0);
}
