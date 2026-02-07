// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "object.h"

#include <sys/pidfd.h>

enum cbuild_target_type {
    CBUILD_TARGET_EXE,
    CBUILD_TARGET_SHAREDLIB,
    CBUILD_TARGET_STATICLIB
};

typedef struct cbuild_target cbuild_target_t;

typedef struct cbuild_custom_target cbuild_custom_target_t;

typedef void (*cbuild_custom_target_func)(const char *outpath, const char *inpath);

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

/**
 * Define a custom target, which depends on file INPATH and produces
 * file OUTPATH by first forking and then calling FUNC.
 *
 * FUNC takes two arguments: outpath and inpath. It returns nothing,
 * and if fails, should call exit() with error code.
 * FUNC does not need to check files modified time, its checked by
 * cbuild before running it.
 *
 * Your FUNC should also print what it's doing. for example, print the
 * command you are exec'ing. otherwise you'll never see if the target
 * was rebuilt or not.
 */
cbuild_custom_target_t *cbuild_create_custom_target(const char *outpath,
                                                    const char *inpath,
                                                    cbuild_custom_target_func func);

/**
 * Compile custom target, as described in docs for
 * cbuild_create_custom_target.
 * Returns -1 on errors, 0 when theres nothing to do (up to date),
 * otherwise returns child PID
 */
pid_t cbuild_custom_target_compile(cbuild_custom_target_t *);
