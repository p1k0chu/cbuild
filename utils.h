// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <stddef.h>

#define DEFINE_ARRAY(name, type) \
    struct cbuild__##name {      \
        type *ptr;               \
        size_t len;              \
        size_t capacity;         \
    };

#define DEFINE_APPENDER_FUN(structname, name, type)                        \
    void cbuild_##structname##_append_##name(struct cbuild_##structname *o, \
                                            type *elements,                \
                                            size_t nelements) {            \
        CBUILD__ENSURE_CAPACITY(o->name, o->name.len + nelements);         \
                                                                           \
        for (size_t i = 0; i < nelements; ++i)                             \
            o->name.ptr[o->name.len++] = elements[i];                      \
        return;                                                          \
    }

void cbuild__ensure_capacity(void **ptr, size_t *capacity, size_t elemsize, size_t desired);

#define CBUILD__ENSURE_CAPACITY(array, desired)        \
    cbuild__ensure_capacity((void **)(&((array).ptr)), \
                            &((array).capacity),       \
                            sizeof(*((array).ptr)),    \
                            (desired))
