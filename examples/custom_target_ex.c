// This file is an example how to use this library.
// It is public domain.
// SPDX-License-Identifier: 0BSD


#define _GNU_SOURCE

#include "custom_target_ex.h"

#include <cbuild.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

static void cat(const char *outpath, const char *inpath);

int main(int argc, char **argv) {
    cbuild_recompile_myself(__FILE__, argv, CBUILD_SELFCOMPILE_FLAGS, NULL);

    cbuild_custom_target_t *t = cbuild_create_custom_target("build_copy.c", "../build.c", cat);

    pid_t p = cbuild_target_compile((void*)t);
    if (p < 0)
        errx(1, "p < 0");
    if (p > 0)
        waitpid(p, NULL, 0);
}

static void cat(const char *outpath, const char *inpath) {
    printf("cat %s > %s", inpath, outpath);

    int infd = open(inpath, O_RDONLY);
    if (infd < 0)
        err(1, "open inpath");

    struct stat instat;
    if (fstat(infd, &instat) < 0)
        err(1, "fstat");

    size_t size = (size_t)instat.st_size;

    int outfd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd < 0)
        err(1, "open outpath");

    do {
        off_t ret = copy_file_range(infd, NULL, outfd, NULL, size, 0);
        if (ret < 0)
            err(1, "copy_file_range");
        size -= (size_t)ret;
    } while (size > 0);

    close(infd);
    close(outfd);
}
