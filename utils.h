// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "cbuild/err.h"

#include <stddef.h>

#define CBUILD_RET_ERR(error, returnval) \
    {                                    \
        cbuild_errno = (error);          \
        return (returnval);              \
    }

#define DEFINE_ARRAY(name, type) \
    struct cbuild__##name {      \
        type *ptr;               \
        size_t len;              \
        size_t capacity;         \
    };

#define DEFINE_APPENDER_FUN(structname, name, type)                                        \
    int cbuild_##structname##_append_##name(struct cbuild_##structname *o, type element) { \
        if (CBUILD__ENSURE_CAPACITY(o->name, o->name.len + 1) < 0)                         \
            CBUILD_RET_ERR(CBUILD_EMALLOC, -1);                                            \
                                                                                           \
        o->name.ptr[o->name.len++] = element;                                              \
        return 0;                                                                          \
    }

int cbuild__ensure_capacity(void **ptr, size_t *capacity, size_t elemsize, size_t desired);

#define CBUILD__ENSURE_CAPACITY(array, desired)        \
    cbuild__ensure_capacity((void **)(&((array).ptr)), \
                            &((array).capacity),       \
                            sizeof(*((array).ptr)),    \
                            (desired))
