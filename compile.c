// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cbuild/compile.h"

#include "mtime.h"
#include "object_private.h"
#include "target_private.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

[[noreturn]] static void run_ar(cbuild_target_t *target);
[[noreturn]] static void run_gcc(cbuild_target_t *target);

static void print_cmd(char **cmd, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        printf("%s ", cmd[i]);
    }
    putchar('\n');
}

pid_t cbuild_target_compile(cbuild_target_t *target) {
    pid_t cpid = fork();
    if (cpid < 0) {
        CBUILD_RET_ERR(CBUILD_EFORK, -1);
    } else if (cpid > 0) {
        return cpid;
    }

    char ineedtocompile = 0;

    for (size_t i = 0; i < target->objs.len; ++i) {
        cbuild_obj_t *obj = target->objs.ptr[i];
        if (cbuild__mtimecmp(obj->outpath, obj->src) < 0) {
            pid_t child = cbuild_obj_compile(obj);
            if (child < 0)
                err(EXIT_FAILURE, "fork");
            int wstatus;
            waitpid(child, &wstatus, 0);

            if (!WIFEXITED(wstatus))
                errx(EXIT_FAILURE, "child exited abnormally");
            int code = WEXITSTATUS(wstatus);
            if (code != 0)
                errx(EXIT_FAILURE, "child returned with error code %d", code);
        }

        if (!ineedtocompile && cbuild__mtimecmp(target->outpath, obj->outpath) < 0) {
            ineedtocompile = 1;
        }
    }

    if (!ineedtocompile)
        exit(EXIT_SUCCESS);

    switch (target->type) {
    case CBUILD_TARGET_STATICLIB:
        run_ar(target);
    default:
        run_gcc(target);
    }
}

pid_t cbuild_obj_compile(cbuild_obj_t *obj) {
    pid_t cpid = fork();
    if (cpid < 0) {
        CBUILD_RET_ERR(CBUILD_EFORK, -1);
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
    cmd[i++] = obj->outpath;
    cmd[i++] = (char *)obj->src;
    cmd[i++] = NULL;

    print_cmd(cmd, i - 1);

    execvp("gcc", cmd);
    err(EXIT_FAILURE, "child failed to exec");
}

[[noreturn]] static void run_gcc(cbuild_target_t *target) {
    const bool isshared = target->type == CBUILD_TARGET_SHAREDLIB;
    const size_t n = (isshared ? 5 : 4) + target->ldflags.len + target->objs.len;
    char *cmd[n];
    size_t i = 0;

    cmd[i++] = "gcc";

    for (size_t j = 0; j < target->objs.len; ++j) {
        cmd[i++] = (char *)target->objs.ptr[j]->outpath;
    }

    if (isshared)
        cmd[i++] = "-shared";

    for (size_t j = 0; j < target->ldflags.len; ++j) {
        cmd[i++] = (char *)target->ldflags.ptr[j];
    }

    cmd[i++] = "-o";
    cmd[i++] = (char *)target->outpath;
    cmd[i++] = NULL;

    print_cmd(cmd, i - 1);

    execvp("gcc", cmd);
    err(EXIT_FAILURE, "failed to exec gcc");
}

static void run_ar(cbuild_target_t *target) {
    char *cmd[4 + target->objs.len];

    cmd[0] = "ar";
    cmd[1] = "rcs";
    cmd[2] = (char *)target->outpath;

    size_t i = 3;
    for (size_t j = 0; j < target->objs.len; ++j)
        cmd[i++] = target->objs.ptr[j]->outpath;

    cmd[i++] = NULL;

    print_cmd(cmd, i - 1);

    execvp("ar", cmd);
    err(EXIT_FAILURE, "failed to exec ar");
}

