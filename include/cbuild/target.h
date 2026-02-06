// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "object.h"

typedef struct cbuild_target cbuild_target_t;

/**
 * Create a target, either an exe or a static/shared library.
 * Name is the filename. varargs are objects 'struct cbuild_obj *',
 * last one must be NULL
 */
cbuild_target_t *cbuild_target_create(const char *name, ...);

void cbuild_target_free(cbuild_target_t *);

int cbuild_target_append_objs(cbuild_target_t *, cbuild_obj_t *);
int cbuild_target_append_ldflags(cbuild_target_t *, const char *);
