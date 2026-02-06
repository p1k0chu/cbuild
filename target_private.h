// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "cbuild/target.h"
#include "utils.h"

DEFINE_ARRAY(obj_array, cbuild_obj_t *);
DEFINE_ARRAY(ldflags, const char *);

typedef struct cbuild_target {
    const char *outpath;
    struct cbuild__obj_array objs;
    struct cbuild__ldflags ldflags;
} cbuild_target_t;
