// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "object.h"
#include "target.h"

typedef struct cbuild_link_target cbuild_link_target_t;
typedef cbuild_link_target_t cbuild_executable_t;
typedef cbuild_link_target_t cbuild_sharedlib_t;
typedef cbuild_link_target_t cbuild_staticlib_t;

/**
 * Create a target, either an exe or a static/shared library.
 * Name is the filename. varargs are objects 'struct cbuild_obj *',
 * last one must be NULL
 */
cbuild_link_target_t *cbuild_link_target_create(enum cbuild_target_type, const char *name, ...);

int cbuild_link_target_append_objs(cbuild_link_target_t *, cbuild_obj_t *);
int cbuild_link_target_append_ldflags(cbuild_link_target_t *, const char *);

/* Macros to create each type of link_target */

#define cbuild_create_executable(name, ...) \
    cbuild_link_target_create(CBUILD_TARGET_EXE, name, __VA_ARGS__)
#define cbuild_create_sharedlib(name, ...) \
    cbuild_link_target_create(CBUILD_TARGET_SHAREDLIB, name, __VA_ARGS__)
#define cbuild_create_staticlib(name, ...) \
    cbuild_link_target_create(CBUILD_TARGET_STATICLIB, name, __VA_ARGS__)

/* Aliases for cbuild_link_target_append... */

#define cbuild_executable_append_objs(exe, obj) cbuild_link_target_append_objs(exe, obj)
#define cbuild_executable_append_ldflags(exe, ldflag) \
    cbuild_link_target_append_ldflags(exe, ldflag)

#define cbuild_sharedlib_append_objs(exe, obj) cbuild_link_target_append_objs(exe, obj)
#define cbuild_sharedlib_append_ldflags(exe, ldflag) \
    cbuild_link_target_append_ldflags(exe, ldflag)

#define cbuild_staticlib_append_objs(exe, obj) cbuild_link_target_append_objs(exe, obj)
#define cbuild_staticlib_append_ldflags(exe, ldflag) \
    cbuild_link_target_append_ldflags(exe, ldflag)
