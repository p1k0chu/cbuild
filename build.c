#include "build.h"

#include "cmdline_parser.h"

#include <cbuild.h>
#include <err.h>
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

    CMDLINE_OPTS_BEGIN;
    CMDLINE_OPT("-s", willcompileself = 0);
    CMDLINE_OPT("-S", willcompileself = 2);
    CMDLINE_OPT("-n", flags |= CBUILD_COMPILE_DRYRUN);
    CMDLINE_OPT("-B", flags |= CBUILD_COMPILE_FORCE);
    CMDLINE_OPT("-h", print_help());
    CMDLINE_OPTS_END;

    // forced self compilation leads to problems, so the library
    // itself exits when it compiles itself with force. this condition
    // will disable force self compilation unless you pass -S.
    int selfflags = flags;
    if (flags & CBUILD_COMPILE_FORCE && willcompileself == 1) {
        warnx("got -B without -S: self compilation wont be forced");
        selfflags &= ~CBUILD_COMPILE_FORCE;
    }

    if (willcompileself) {
        cbuild_recompile_myself(__FILE__,
                                argv,
                                selfflags,
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
        "-h - show help\n",
        prog);
    exit(0);
}
