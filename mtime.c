// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

int cbuild__mtimecmp(const char *p1, const char *p2) {
    struct stat s1, s2;
    int s1exists;

    int c = stat(p1, &s1);
    if (c < 0) {
        if (errno == ENOENT)
            s1exists = 0;
        else
            err(EXIT_FAILURE, "stat(\"%s\", ...)", p1);
    } else
        s1exists = 1;
    c = stat(p2, &s2);
    if (c < 0) {
        if (errno == ENOENT)
            return s1exists;
        else
            err(EXIT_FAILURE, "stat(\"%s\", ...)", p2);
    } else if (!s1exists) {
        return -1;
    }

    long delta = s1.st_mtim.tv_sec - s2.st_mtim.tv_sec;
    if (delta != 0)
        return (int)delta;
    return (int)(s1.st_mtim.tv_nsec - s2.st_mtim.tv_nsec);
}
