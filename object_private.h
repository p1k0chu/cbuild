// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "cbuild/object.h"
#include "utils.h"

DEFINE_ARRAY(cflags, const char *);

typedef struct cbuild_obj {
    char *outpath;
    const char *src;

    struct cbuild__cflags cflags;
} cbuild_obj_t;
