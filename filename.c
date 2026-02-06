// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <stdlib.h>
#include <string.h>

char *cbuild_changestrext(const char *s, const char *ext) {
    char *dot = strrchr(s, '.');
    size_t sn;

    if (dot != NULL) {
        sn = (size_t)(dot - s);
    } else {
        sn = strlen(s);
    }

    size_t extn = strlen(ext);
    size_t n = sn + 1 + extn;

    char *new = malloc(n + 1);
    if (new == NULL)
        return NULL;

    char *p;
    p = mempcpy(new, s, sn);
    *p++ = '.';
    p = mempcpy(p, ext, extn);

    *p = 0;
    return new;
}
