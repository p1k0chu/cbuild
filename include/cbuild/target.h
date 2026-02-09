// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <sys/pidfd.h>

enum cbuild_target_type {
    CBUILD_TARGET_EXE,
    CBUILD_TARGET_SHAREDLIB,
    CBUILD_TARGET_STATICLIB,
    CBUILD_TARGET_OBJECT,
    CBUILD_TARGET_CUSTOM
};

// base struct for all targets
typedef struct cbuild_target cbuild_target_t;

void cbuild_target_free(cbuild_target_t *);
