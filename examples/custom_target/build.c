// This file is an example how to use this library.
// It is public domain.
// SPDX-License-Identifier: 0BSD

#include "build.h"

#include <cbuild.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

const char file1[] =
    "#include <stdio.h>\n"
    "int answer();\n"
    "int main() {"
    "printf(\"%d\\n\", answer());\n"
    "}";
const char file2[] = "int answer() { return 42; }";

static void makefile1(const char *outpath, const char *);
static void makefile2(const char *outpath, const char *);

int main(int argc, char **argv) {
    cbuild_recompile_myself(__FILE__, argv, CBUILD_SELFCOMPILE_FLAGS, NULL);

    cbuild_target_t *t = (void *)cbuild_create_custom_target("file1.c", NULL, makefile1);
    cbuild_target_t *t2 = (void *)cbuild_create_custom_target("file2.c", NULL, makefile2);

    cbuild_obj_t *o = cbuild_obj_create("file1.c", NULL);
    cbuild_obj_t *o2 = cbuild_obj_create("file2.c", NULL);

    cbuild_target_append_deps((void *)o, &t, 1);
    cbuild_target_append_deps((void *)o2, &t2, 1);

    cbuild_executable_t *exe = cbuild_create_executable("program", o, o2, NULL);

    pid_t p = cbuild_target_compile((void *)exe);
    if (p > 0)
        waitpid(p, NULL, 0);
}
static void makefile1(const char *outpath, const char *) {
    printf("generating file1.c\n");

    int fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        err(1, "open");

    if (write(fd, file1, sizeof(file1) / sizeof(*file1) - 1) < 0)
        err(1, "write");

    if (close(fd) < 0)
        err(1, "close");
}

static void makefile2(const char *outpath, const char *) {
    printf("generating file2.c\n");

    int fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        err(1, "open");

    if (write(fd, file2, sizeof(file2) / sizeof(*file2) - 1) < 0)
        err(1, "write");

    if (close(fd) < 0)
        err(1, "close");
}
