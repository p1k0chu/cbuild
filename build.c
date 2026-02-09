#include "build.h"

#include <cbuild.h>
#include <stdio.h>
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
    "custom_target.c",
};

#define COMMON_CFLAGS                                                                 \
    "-Wall", "-Wextra", "-iquote", "./include", "-std=gnu23", "-Werror=implicit-int", \
        "-Wconversion", "-Werror=implicit-fallthrough"

int main(int argc __attribute__((unused)), char **argv) {
    cbuild_recompile_myself(__FILE__, argv, "-Wall", "-Wextra", CBUILD_SELFCOMPILE_FLAGS, NULL);

    cbuild_staticlib_t *lib = cbuild_create_staticlib("libcbuild.a", NULL);
    if (lib == NULL)
        return 1;

    const size_t n = sizeof(srcs) / sizeof(*srcs);
    for (size_t i = 0; i < n; ++i) {
        cbuild_obj_t *obj = cbuild_obj_create(srcs[i], COMMON_CFLAGS, NULL);
        if (obj == NULL)
            return 1;
        if (cbuild_staticlib_append_objs(lib, obj) < 0)
            return 1;
    }
    int ws;
    pid_t cpid = cbuild_target_compile((void *)lib);
    if (cpid < 0)
        return 1;
    if (cpid == 0)
        return 0;

    waitpid(cpid, &ws, 0);
    if (!WIFEXITED(ws))
        return 1;
    return WEXITSTATUS(ws);
}
