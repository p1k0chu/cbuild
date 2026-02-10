// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#define _GNU_SOURCE

#include "cbuild/pkgconfig.h"

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

static inline size_t char_count_str(const char *haystack, char hay);
static char *exec_pkg_config(char *flag, const char **libs, size_t nlibs);

size_t cbuild_pkgconfig(char ***ret,
                        enum pkg_config_what what,
                        const char **libs,
                        size_t nlibs,
                        void **freeptr) {
    char *flag;
    switch (what) {
    case PKG_CONFIG_LIBS:
        flag = "--libs";
        break;
    case PKG_CONFIG_CFLAGS:
        flag = "--cflags";
        break;
    default:
        errx(1, "pkg_config_what value is invalid (not of enum)");
    }
    char *p = exec_pkg_config(flag, libs, nlibs);

    const size_t bufsize = char_count_str(p, ' ') + 1;

    char **buf = malloc(bufsize * sizeof(*buf));
    if (buf == NULL) {
        err(1, "malloc");
    }
    buf[0] = strtok(p, " ");

    for (size_t i = 1; i < bufsize; ++i) {
        char *t = strtok(NULL, " ");
        if (t == NULL)
            break;
        buf[i] = t;
    }
    *ret = buf;
    if (freeptr != NULL)
        *freeptr = p;
    return bufsize;
}

static char *exec_pkg_config(char *flag, const char **libs, size_t libs_n) {
    int fds[2];
    if (pipe(fds) < 0)
        err(1, "failed to pipe");

    pid_t cpid = fork();
    if (cpid < 0) {
        err(1, "fork");
    } else if (cpid > 0) {
        FILE *fpipe = fdopen(fds[0], "r");
        if (!fpipe)
            err(1, "fdopen");
        close(fds[1]);

        char *p = NULL;
        size_t slen = 0;

        if (getline(&p, &slen, fpipe) < 0) {
            if (ferror(fpipe))
                err(1, "getline");
        }
        fclose(fpipe);

        int ws;
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws)) {
            errx(1, "child died to a signal");
        } else {
            int status = WEXITSTATUS(ws);
            if (status != 0)
                errx(1, "child exited with error code %d", status);
        }

        if (p) {
            char *nl = strchr(p, '\n');
            if (nl)
                *nl = 0;
        }

        return p;
    }

    dup2(fds[1], STDOUT_FILENO);
    close(fds[0]);
    close(fds[1]);

    size_t cmdlen = 3 + libs_n;
    char *cmd[cmdlen];
    size_t i = 0;

    cmd[i++] = "pkg-config";
    if (flag)
        cmd[i++] = flag;

    for (size_t j = 0; j < libs_n; ++j) {
        cmd[i++] = (char *)libs[j];
    }

    cmd[i++] = NULL;

    execvp(cmd[0], cmd);
    err(1, "failed to exec pkg-config");
}

static inline size_t char_count_str(const char *haystack, char hay) {
    size_t i = 0;
    for (; *haystack; ++haystack)
        if (*haystack == hay)
            ++i;
    return i;
}
