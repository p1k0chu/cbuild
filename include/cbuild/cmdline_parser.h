// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <string.h>

#define CMDLINE_OPTS_BEGIN for (int i = 1; i < argc; ++i) {
#define CMDLINE_OPTS_END }

#define CMDLINE_OPT2(_short, _long, action)                                   \
    do {                                                                     \
        if (strcmp(argv[i], (_short)) == 0 || strcmp(argv[i], (_long)) == 0) \
            action;                                                          \
    } while (0)

#define CMDLINE_OPT1(str, action)         \
    do {                                 \
        if (strcmp(argv[i], (str)) == 0) \
            action;                      \
    } while (0)

#define EXPAND(x) x
#define CMDLINE_OPT_GET_MACRO(_1, _2, _3, name, ...) name
#define CMDLINE_OPT(...) EXPAND(CMDLINE_OPT_GET_MACRO(__VA_ARGS__, CMDLINE_OPT2, CMDLINE_OPT1)(__VA_ARGS__))


