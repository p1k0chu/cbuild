// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cbuild/compile.h"

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

void cbuild_recompile_myself(const char *sourcepath, char **argv, ...) {
    struct stat stselflink, stsrc;
    if (lstat("/proc/self/exe", &stselflink) < 0)
        err(EXIT_FAILURE, "can't stat /proc/self/exe");

    if (stat(sourcepath, &stsrc) < 0) {
        if (errno == ENOENT) {
            warn("source path doesn't exist: %s", sourcepath);
            return;
        } else {
            err(EXIT_FAILURE, "couldn't lstat source path: %s", sourcepath);
        }
    }

    size_t bufsize;
    if (stselflink.st_size <= 0)
        bufsize = PATH_MAX;
    else
        bufsize = (size_t)stselflink.st_size + 1;

    char *buf = malloc((size_t)bufsize);
    if (buf == NULL)
        err(EXIT_FAILURE, "malloc");

    ssize_t nb = readlink("/proc/self/exe", buf, (size_t)bufsize);
    if (nb < 0)
        err(EXIT_FAILURE, "readlink");

    if ((size_t)nb == bufsize) {
        warnx("readlink may have truncated the path: %*s", (int)nb, buf);
        buf[nb - 1] = 0;
    } else {
        buf[nb] = 0;
    }

    struct stat stbin;
    if (stat(buf, &stbin) < 0) {
        if (errno == ENOENT)
            goto recompile;
        else
            err(EXIT_FAILURE, "couldn't stat binary /proc/self/exe is pointing at: %s", buf);
    }

    long delta = stbin.st_mtim.tv_sec - stsrc.st_mtim.tv_sec;
    if (delta == 0)
        delta = stbin.st_mtim.tv_nsec - stsrc.st_mtim.tv_nsec;

    if (delta >= 0)
        return;

recompile:

    cbuild_obj_t *obj = cbuild_obj_create(sourcepath, NULL);
    cbuild_target_t *exe = cbuild_create_executable(buf, obj, NULL);

    va_list vlist;
    va_start(vlist, argv);
    const char *p;
    for (;;) {
        p = va_arg(vlist, const char *);
        if (p == NULL)
            break;

        cbuild_obj_append_cflags(obj, p);
        cbuild_target_append_ldflags(exe, p);
    }
    va_end(vlist);

    pid_t cpid = cbuild_target_compile(exe);
    if (cpid < 0)
        err(EXIT_FAILURE, "fork");
    int wstatus;
    waitpid(cpid, &wstatus, 0);
    if (!WIFEXITED(wstatus))
        errx(EXIT_FAILURE, "self compilation exited abnormally");

    int s = WEXITSTATUS(wstatus);
    if (s != 0)
        errx(EXIT_FAILURE, "self compilation exited with error code %d", s);

    for (size_t i = 0; argv[i] != NULL; ++i) {
        printf("%s ", argv[i]);
    }
    putchar('\n');

    execv(buf, argv);
    err(EXIT_FAILURE, "execv: %s", buf);
}
