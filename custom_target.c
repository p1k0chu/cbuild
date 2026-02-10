// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "mtime.h"
#include "target_private.h"

#include <err.h>
#include <stdlib.h>
#include <unistd.h>

cbuild_custom_target_t *cbuild_create_custom_target(const char *outpath,
                                                    const char *inpath,
                                                    cbuild_custom_target_func func) {
    cbuild_custom_target_t *t = calloc(1, sizeof(*t));
    if (t == NULL)
        err(1, "calloc");

    t->base.outpath = outpath;
    t->base.type = CBUILD_TARGET_CUSTOM;
    t->inpath = inpath;
    t->func = func;

    return t;
}

pid_t cbuild__custom_target_compile(cbuild_custom_target_t *target) {
    pid_t cpid = fork();
    if (cpid < 0) {
        err(1, "fork");
    } else if (cpid > 0) {
        return cpid;
    }

    target->func(target->base.outpath, target->inpath);
    exit(0);
}

