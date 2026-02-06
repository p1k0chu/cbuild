// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "cbuild/misc.h"
#include "object_private.h"
#include "utils.h"

#include <stdarg.h>
#include <stdlib.h>

DEFINE_APPENDER_FUN(obj, cflags, const char *)

cbuild_obj_t *cbuild_obj_create(const char *cfile, ...) {
    cbuild_obj_t *o = calloc(1, sizeof(*o));
    if (o == NULL)
        goto error;

    o->outpath = cbuild_changestrext(cfile, "o");
    if (o->outpath == NULL)
        goto error;

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
            if (cbuild_obj_append_cflags(o, flag) < 0)
                goto error;
        }
    };

error:
    free(o);
    CBUILD_RET_ERR(CBUILD_EMALLOC, NULL);
}

void cbuild_obj_free(cbuild_obj_t *o) {
    free(o->outpath);
    free(o->cflags.ptr);
    free(o);
}
