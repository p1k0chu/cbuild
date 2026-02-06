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

    char *cmd[4 + target->ldflags.len + target->objs.len];
    size_t i = 0;

    cmd[i++] = "gcc";

    for (size_t j = 0; j < target->objs.len; ++j) {
        cmd[i++] = (char *)target->objs.ptr[j]->outpath;
    }

    for (size_t j = 0; j < target->ldflags.len; ++j) {
        cmd[i++] = (char *)target->ldflags.ptr[j];
    }

    cmd[i++] = "-o";
    cmd[i++] = (char *)target->outpath;

    cmd[i++] = NULL;

    for (size_t j = 0; j < i - 1; ++j) {
        printf("%s ", cmd[j]);
    }
    putchar('\n');

    execvp("gcc", cmd);
    err(EXIT_FAILURE, "child failed to exec");
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

    for (size_t j = 0; j < i - 1; ++j) {
        printf("%s ", cmd[j]);
    }
    putchar('\n');

    execvp("gcc", cmd);
    err(EXIT_FAILURE, "child failed to exec");
}
