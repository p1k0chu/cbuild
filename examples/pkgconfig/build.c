// This file is an example of how to use pkgconfig utility function in
// cbuild. it is public domain.
// SPDX-License-Identifier: 0BSD

#define _DEFAULT_SOURCE

#include "build.h"

#include <cbuild.h>
#include <cbuild/pkgconfig.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    cbuild_recompile_myself(__FILE__, argv, 0, CBUILD_SELFCOMPILE_FLAGS, NULL);

    // use math.pc in this example's directory (using any other
    // library isn't guaranteed to work on all linuxes)
    setenv("PKG_CONFIG_PATH", ".", 1);

    const char *libs[] = {"math"};
    const size_t nlibs = sizeof(libs) / sizeof(*libs);

    char **cflags;
    const int ncflags = cbuild_pkgconfig(&cflags, PKG_CONFIG_CFLAGS, libs, nlibs, NULL);

    char **ldflags;
    const int nldflags = cbuild_pkgconfig(&ldflags, PKG_CONFIG_LIBS, libs, nlibs, NULL);

    cbuild_obj_t *main = cbuild_obj_create("main.c", NULL);
    cbuild_obj_append_cflags(main, (const char **)cflags, ncflags);

    cbuild_executable_t *exe = cbuild_create_executable("program", main, NULL);
    cbuild_executable_append_ldflags(exe, (const char **)ldflags, nldflags);

    pid_t cpid = cbuild_target_compile((void *)exe, 0);
    if (cpid > 0) {
        int ws;
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            return 1;
        return WEXITSTATUS(ws);
    }
}
