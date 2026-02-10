// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "cbuild/custom_target.h"
#include "cbuild/target.h"
#include "utils.h"

DEFINE_ARRAY(target_array, cbuild_target_t *);
DEFINE_ARRAY(ldflags, const char *);
DEFINE_ARRAY(cflags, const char *);

// flags:
/// Only used for dry runs to keep track of targets. Real compilations
/// ignore this (but still set)
#define CBUILD_TARGET_ISCOMPILED 1

struct cbuild_target {
    int flags;
    enum cbuild_target_type type;

    const char *outpath;
    struct cbuild__target_array deps;
};

struct cbuild_link_target {
    cbuild_target_t base;

    struct cbuild__ldflags ldflags;
};

struct cbuild_obj {
    cbuild_target_t base;

    const char *src;

    struct cbuild__cflags cflags;
};

struct cbuild_custom_target {
    cbuild_target_t base;

    const char *inpath;
    cbuild_custom_target_func func;
    void *data;
};

pid_t cbuild__custom_target_compile(cbuild_custom_target_t *target, int flags);
