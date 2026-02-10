// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cbuild/object.h"

#include "cbuild/misc.h"
#include "target_private.h"
#include "utils.h"

#include <stdarg.h>
#include <stdlib.h>
#include <err.h>

DEFINE_APPENDER_FUN(obj, cflags, const char *)

cbuild_obj_t *cbuild_obj_create(const char *cfile, ...) {
    cbuild_obj_t *o = calloc(1, sizeof(*o));
    if (o == NULL)
        err(1, "calloc");

    o->base.type = CBUILD_TARGET_OBJECT;
    o->base.outpath = cbuild_changestrext(cfile, "o");

    o->src = cfile;

    va_list vlist;
    va_start(vlist, cfile);

    const char *flag;
    for (;;) {
        flag = va_arg(vlist, const char *);
        if (flag == NULL) {
            va_end(vlist);
            return o;
        } else {
            cbuild_obj_append_cflags(o, &flag, 1);
        }
    };
}

