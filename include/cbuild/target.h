// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "object.h"

enum cbuild_target_type {
    CBUILD_TARGET_EXE,
    CBUILD_TARGET_SHAREDLIB,
    CBUILD_TARGET_STATICLIB
};

typedef struct cbuild_target cbuild_target_t;

/**
 * Create a target, either an exe or a static/shared library.
 * Name is the filename. varargs are objects 'struct cbuild_obj *',
 * last one must be NULL
 */
cbuild_target_t *cbuild_target_create(enum cbuild_target_type, const char *name, ...);

#define cbuild_create_executable(name, ...) \
    cbuild_target_create(CBUILD_TARGET_EXE, name, __VA_ARGS__)
#define cbuild_create_sharedlib(name, ...) \
    cbuild_target_create(CBUILD_TARGET_SHAREDLIB, name, __VA_ARGS__)
#define cbuild_create_staticlib(name, ...) \
    cbuild_target_create(CBUILD_TARGET_STATICLIB, name, __VA_ARGS__)

void cbuild_target_free(cbuild_target_t *);

int cbuild_target_append_objs(cbuild_target_t *, cbuild_obj_t *);
int cbuild_target_append_ldflags(cbuild_target_t *, const char *);
