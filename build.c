#include "build.h"

#include <cbuild.h>
#include <string.h>
#include <wait.h>

const char *srcs[] = {
    "compile.c",
    "compilemyself.c",
    "error.c",
    "filename.c",
    "mtime.c",
    "object.c",
    "target.c",
    "utils.c",
};

#define COMMON_CFLAGS                                                                 \
    "-Wall", "-Wextra", "-iquote", "./include", "-std=gnu23", "-Werror=implicit-int", \
        "-Wconversion", "-Werror=implicit-fallthrough"

int main(int argc __attribute__((unused)), char **argv) {
    cbuild_recompile_myself(__FILE__, argv, "-Wall", "-Wextra", CBUILD_SELFCOMPILE_FLAGS, NULL);

    cbuild_target_t *liba = cbuild_create_staticlib("libcbuild.a", NULL);
    if (liba == NULL)
        return 1;

    cbuild_target_t *libso = cbuild_create_sharedlib("libcbuild.so", NULL);

    const size_t n = sizeof(srcs) / sizeof(*srcs);
    for (size_t i = 0; i < n; ++i) {
        cbuild_obj_t *obj = cbuild_obj_create(srcs[i], COMMON_CFLAGS, NULL);
        if (obj == NULL)
            return 1;
        if (cbuild_target_append_objs(liba, obj) < 0)
            return 1;
        if (cbuild_target_append_objs(libso, obj) < 0)
            return 1;
    }

    int ws;
    int status = 0;
    pid_t cpid = cbuild_target_compile(liba);
    if (cpid < 0) {
        return 1;
    } else if (cpid > 0) {
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        status = WEXITSTATUS(ws);
    }

    cpid = cbuild_target_compile(libso);
    if (cpid < 0) {
        return 1;
    } else if (cpid > 0) {
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        status |= WEXITSTATUS(ws);
    }

    return status;
}
