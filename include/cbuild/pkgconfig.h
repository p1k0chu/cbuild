// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <stddef.h>

enum pkg_config_what {
    // pass --libs to pkgconfig
    PKG_CONFIG_LIBS,
    // pass --cflags to pkgconfig
    PKG_CONFIG_CFLAGS,
};

/**
 * calls pkg-config, and splits the flags into tokens
 * (delim=space). writes address of the array of strings to RET, and
 * returns the number of tokens in that array.
 *
 * when freeptr is not NULL, writes the address you can pass to free()
 * function to free the memory. this makes every string in ret
 * unusable. after this you also should free ret.
 */
size_t cbuild_pkgconfig(char ***ret,
                        enum pkg_config_what what,
                        const char **libs,
                        size_t nlibs,
                        void **freeptr);
