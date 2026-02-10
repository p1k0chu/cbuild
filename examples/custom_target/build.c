// This file is an example how to use this library.
// It is public domain.
// SPDX-License-Identifier: 0BSD

#include "build.h"

#include <cbuild.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

char file1[] =
    "#include <stdio.h>\n"
    "int answer();\n"
    "int main() {"
    "printf(\"%d\\n\", answer());\n"
    "}";
char file2[] = "int answer() { return 42; }";

static void makefile(const char *outpath, const char *, int flags, void *);

int main(int argc, char **argv) {
    cbuild_recompile_myself(__FILE__, argv, 0, CBUILD_SELFCOMPILE_FLAGS, NULL);

    cbuild_target_t *t = (void *)cbuild_create_custom_target("file1.c", NULL, makefile, file1);
    cbuild_target_t *t2 = (void *)cbuild_create_custom_target("file2.c", NULL, makefile, file2);

    cbuild_obj_t *o = cbuild_obj_create("file1.c", NULL);
    cbuild_obj_t *o2 = cbuild_obj_create("file2.c", NULL);

    cbuild_target_append_deps((void *)o, &t, 1);
    cbuild_target_append_deps((void *)o2, &t2, 1);

    cbuild_executable_t *exe = cbuild_create_executable("program", o, o2, NULL);

    pid_t p = cbuild_target_compile((void *)exe, 0);
    if (p > 0)
        waitpid(p, NULL, 0);
}

static void makefile(const char *outpath, const char *, int flags, void *data) {
    printf("generating file1.c\n");

    if (flags & CBUILD_COMPILE_DRYRUN)
        return;

    int fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        err(1, "open");

    if (write(fd, data, strlen(data)) < 0)
        err(1, "write");

    if (close(fd) < 0)
        err(1, "close");
}
