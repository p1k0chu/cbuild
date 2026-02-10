// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <stddef.h>
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

/**
 * Add a dependency on another target, e.g. an object.
 * This means the dependencies will be built before this target, and
 * they will be added to compilation command appropriately
 * (e.g. custom targets and executables are ignored, libraries turn
 * into linker flags, ...)
 */
void cbuild_target_append_deps(cbuild_target_t *target, cbuild_target_t **deps, size_t ndeps);

void cbuild_target_free(cbuild_target_t *);
