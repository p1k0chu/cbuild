// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "target_private.h"

#include <stdarg.h>
#include <stdlib.h>

DEFINE_APPENDER_FUN(target, objs, cbuild_obj_t *);
DEFINE_APPENDER_FUN(target, ldflags, const char *);

cbuild_target_t *cbuild_target_create(const char *name, ...) {
    cbuild_target_t *p = calloc(1, sizeof(*p));
    if (p == NULL)
        CBUILD_RET_ERR(CBUILD_EMALLOC, NULL);

    p->outpath = name;

    va_list vlist;
    va_start(vlist, name);
    cbuild_obj_t *o;

    for (;;) {
        o = va_arg(vlist, cbuild_obj_t *);
        if (o == NULL) {
            va_end(vlist);
            return p;
        } else {
            cbuild_target_append_objs(p, o);
        }
    }
}

void cbuild_target_free(cbuild_target_t *p) {
    free(p->ldflags.ptr);
    free(p->objs.ptr);
    free(p);
}
