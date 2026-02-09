// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "cbuild/custom_target.h"
#include "cbuild/object.h"
#include "cbuild/target.h"
#include "utils.h"

DEFINE_ARRAY(obj_array, cbuild_obj_t *);
DEFINE_ARRAY(ldflags, const char *);

struct cbuild_target {
    enum cbuild_target_type type;

    const char *outpath;
};

struct cbuild_link_target {
    cbuild_target_t base;

    struct cbuild__obj_array objs;
    struct cbuild__ldflags ldflags;
};

DEFINE_ARRAY(cflags, const char *);

struct cbuild_obj {
    cbuild_target_t base;

    const char *src;

    struct cbuild__cflags cflags;
};

struct cbuild_custom_target {
    cbuild_target_t base;

    const char *inpath;
    cbuild_custom_target_func func;
};

pid_t cbuild__custom_target_compile(cbuild_custom_target_t *target);
