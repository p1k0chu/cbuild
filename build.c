#include "build.h"

#include <cbuild.h>
#include <stdio.h>
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

int main(int argc __attribute__((unused)), char **argv) {
    cbuild_recompile_myself(__FILE__, argv, "-Wall", "-Wextra", CBUILD_SELFCOMPILE_FLAGS, NULL);

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
    pid_t cpid = cbuild_target_compile((void *)liba);
    if (cpid > 0) {
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        status = WEXITSTATUS(ws);
    }

    cpid = cbuild_target_compile((void *)libso);
    if (cpid > 0) {
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        status |= WEXITSTATUS(ws);
    }

    return status;
}
