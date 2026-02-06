// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "utils.h"

#include "cbuild/err.h"

#include <stddef.h>
#include <stdlib.h>

int cbuild__ensure_capacity(void **ptr, size_t *capacity, size_t elemsize, size_t desired) {
    if (desired <= *capacity)
        return 0;

    size_t newsize = desired * 2 + 1;
    void *p = realloc(*ptr, newsize * elemsize);
    if (p == NULL)
        CBUILD_RET_ERR(CBUILD_EMALLOC, -1);

    *ptr = p;
    *capacity = newsize;
    return 0;
}
