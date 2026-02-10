// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cbuild/compile.h"

#include "cbuild/link_target.h"
#include "mtime.h"
#include "target_private.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

[[noreturn]] static void run_ar(cbuild_link_target_t *target, int flags);
[[noreturn]] static void run_gcc(cbuild_link_target_t *target, int flags);

static void print_cmd(char **cmd, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%s ", cmd[i]);
    }
    putchar('\n');
}

static pid_t cbuild_obj_compile(cbuild_obj_t *obj, int flags);
static pid_t cbuild_link_target_compile(cbuild_link_target_t *target, int flags);

pid_t cbuild_target_compile(cbuild_target_t *target, int flags) {
    if (flags & CBUILD_COMPILE_DRYRUN) {
        if (target->flags & CBUILD_TARGET_ISCOMPILED)
            return -69;
    }
    char updateddeps = flags & CBUILD_COMPILE_FORCE;
    target->flags |= CBUILD_TARGET_ISCOMPILED;

    for (size_t i = 0; i < target->deps.len; ++i) {
        cbuild_target_t *obj = target->deps.ptr[i];
        pid_t child = cbuild_target_compile(obj, flags);
        if (child == -69 && (flags & CBUILD_COMPILE_DRYRUN)) {
            updateddeps |= 1;
        } else if (child < 0)
            err(EXIT_FAILURE, "fork");
        else if (child > 0) {
            updateddeps |= 1;
            int wstatus;
            waitpid(child, &wstatus, 0);

            if (!WIFEXITED(wstatus))
                errx(EXIT_FAILURE, "child exited abnormally");
            int code = WEXITSTATUS(wstatus);
            if (code != 0)
                errx(EXIT_FAILURE, "child returned with error code %d", code);
        }

        if (!updateddeps) {
            updateddeps |= cbuild__mtimecmp(target->outpath, obj->outpath) < 0;
        }
    }

    switch (target->type) {
    case CBUILD_TARGET_EXE:
    case CBUILD_TARGET_SHAREDLIB:
    case CBUILD_TARGET_STATICLIB:
        if (updateddeps) {
            return cbuild_link_target_compile((cbuild_link_target_t *)target, flags);
        }
        break;
    case CBUILD_TARGET_OBJECT:
        if (updateddeps || cbuild__mtimecmp(target->outpath, ((cbuild_obj_t *)target)->src) < 0) {
            return cbuild_obj_compile((cbuild_obj_t *)target, flags);
        }
        break;
    case CBUILD_TARGET_CUSTOM:
        cbuild_custom_target_t *ct = (void *)target;
        if (updateddeps ||
            (ct->inpath == NULL || cbuild__mtimecmp(target->outpath, ct->inpath) < 0)) {
            return cbuild__custom_target_compile((cbuild_custom_target_t *)target, flags);
        }
        break;
    default:
        errx(1, "target type is invalid value (not enum)");
    }
    return 0;
}

static pid_t cbuild_link_target_compile(cbuild_link_target_t *target, int flags) {
    pid_t cpid = fork();
    if (cpid < 0) {
        err(1, "fork");
    } else if (cpid > 0) {
        return cpid;
    }

    switch (target->base.type) {
    case CBUILD_TARGET_STATICLIB:
        run_ar(target, flags);
    default:
        run_gcc(target, flags);
    }
}

static pid_t cbuild_obj_compile(cbuild_obj_t *obj, int flags) {
    pid_t cpid = fork();
    if (cpid < 0) {
        err(1, "fork");
    } else if (cpid > 0) {
        return cpid;
    }

    char *cmd[6 + obj->cflags.len];
    size_t i = 0;

    cmd[i++] = "gcc";

    for (size_t j = 0; j < obj->cflags.len; ++j) {
        cmd[i++] = (char *)obj->cflags.ptr[j];
    }

    cmd[i++] = "-c";
    cmd[i++] = "-o";
    cmd[i++] = (char *)obj->base.outpath;
    cmd[i++] = (char *)obj->src;
    cmd[i++] = NULL;

    print_cmd(cmd, i - 1);

    if (flags & CBUILD_COMPILE_DRYRUN)
        exit(0);

    execvp("gcc", cmd);
    err(EXIT_FAILURE, "child failed to exec");
}

[[noreturn]] static void run_gcc(cbuild_link_target_t *target, int flags) {
    const bool isshared = target->base.type == CBUILD_TARGET_SHAREDLIB;
    const size_t n = (isshared ? 5 : 4) + target->ldflags.len + target->base.deps.len;
    char *cmd[n];
    size_t i = 0;

    cmd[i++] = "gcc";

    // separate loops, because objects go first, before potential
    // libraries

    for (size_t j = 0; j < target->base.deps.len; ++j) {
        cbuild_target_t *d = target->base.deps.ptr[j];
        switch (d->type) {
        case CBUILD_TARGET_EXE:
        case CBUILD_TARGET_SHAREDLIB:
        case CBUILD_TARGET_STATICLIB:
        case CBUILD_TARGET_CUSTOM:
            break;
        case CBUILD_TARGET_OBJECT:
            cmd[i++] = (char *)d->outpath;
            break;
        }
    }

    for (size_t j = 0; j < target->base.deps.len; ++j) {
        cbuild_target_t *d = target->base.deps.ptr[j];
        switch (d->type) {
        case CBUILD_TARGET_SHAREDLIB:
        case CBUILD_TARGET_STATICLIB:
            // make a -l:libexample.a kind of argument out of d->outpath

            const ssize_t size = snprintf(NULL, 0, "-l:%s", d->outpath);
            if (size < 0)
                err(1, "snprintf");

            // we never free anything because of exec/exit
            char *buf = malloc((size_t)size + 1);
            if (buf == NULL)
                err(1, "malloc");
            const ssize_t size2 = snprintf(buf, (size_t)size + 1, "-l:%s", d->outpath);
            if (size2 < 0)
                err(1, "snprintf");
            if (size2 >= size)
                warnx("snprintf truncated: %s", buf);

            cmd[i++] = buf;
            break;
        case CBUILD_TARGET_OBJECT:
        case CBUILD_TARGET_EXE:
        case CBUILD_TARGET_CUSTOM:
            break;
        }
    }

    if (isshared)
        cmd[i++] = "-shared";

    for (size_t j = 0; j < target->ldflags.len; ++j) {
        cmd[i++] = (char *)target->ldflags.ptr[j];
    }

    cmd[i++] = "-o";
    cmd[i++] = (char *)target->base.outpath;
    cmd[i++] = NULL;

    print_cmd(cmd, i - 1);

    if (flags & CBUILD_COMPILE_DRYRUN)
        exit(0);

    execvp("gcc", cmd);
    err(EXIT_FAILURE, "failed to exec gcc");
}

static void run_ar(cbuild_link_target_t *target, int flags) {
    char *cmd[4 + target->base.deps.len];

    cmd[0] = "ar";
    cmd[1] = "rcs";
    cmd[2] = (char *)target->base.outpath;

    size_t i = 3;
    for (size_t j = 0; j < target->base.deps.len; ++j) {
        cbuild_target_t *d = target->base.deps.ptr[j];
        if (d->type == CBUILD_TARGET_OBJECT)
            cmd[i++] = (char *)d->outpath;
    }

    cmd[i++] = NULL;

    print_cmd(cmd, i - 1);

    if (flags & CBUILD_COMPILE_DRYRUN)
        exit(0);

    execvp("ar", cmd);
    err(EXIT_FAILURE, "failed to exec ar");
}
