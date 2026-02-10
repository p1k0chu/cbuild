// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cbuild/link_target.h"
#include "target_private.h"

#include <err.h>
#include <stdarg.h>
#include <stdlib.h>

DEFINE_APPENDER_FUN(target, deps, cbuild_target_t *);
DEFINE_APPENDER_FUN(link_target, ldflags, const char *);

cbuild_link_target_t *
cbuild_link_target_create(enum cbuild_target_type type, const char *name, ...) {
    switch (type) {
    case CBUILD_TARGET_EXE:
    case CBUILD_TARGET_SHAREDLIB:
    case CBUILD_TARGET_STATICLIB:
        break;
    case CBUILD_TARGET_OBJECT:
    case CBUILD_TARGET_CUSTOM:
    default:
        errx(1, "target type is invalid");
    }

    cbuild_link_target_t *p = calloc(1, sizeof(*p));
    if (p == NULL)
        err(1, "calloc");

    p->base.outpath = name;
    p->base.type = type;

    va_list vlist;
    va_start(vlist, name);
    cbuild_target_t *o;

    for (;;) {
        o = va_arg(vlist, cbuild_target_t *);
        if (o == NULL) {
            va_end(vlist);
            return p;
        } else {
            cbuild_target_append_deps((void *)p, &o, 1);
        }
    }
}

void cbuild_target_free(cbuild_target_t *p) {
    switch (p->type) {
    case CBUILD_TARGET_EXE:
    case CBUILD_TARGET_SHAREDLIB:
    case CBUILD_TARGET_STATICLIB:
        cbuild_link_target_t *t = (void *)p;
        free(t->ldflags.ptr);
        break;
    case CBUILD_TARGET_OBJECT:
        cbuild_obj_t *o = (void *)p;
        // is a malloc'ed pointer.
        free((void *)o->base.outpath);
        free(o->cflags.ptr);
        break;
    case CBUILD_TARGET_CUSTOM:
        // nothing to free
        break;
    }
    free(p->deps.ptr);
    free(p);
}
