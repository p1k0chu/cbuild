// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <stddef.h>

typedef struct cbuild_obj cbuild_obj_t;

/**
 * Create an object file (.o) from a cfile (.c), with compile flags
 * from vararg.
 * Vararg must end in NULL.
 * Returns a pointer to struct, which must be freed after no longer
 * needed
 */
cbuild_obj_t *cbuild_obj_create(const char *cfile, ...);

/**
 * Append cflags.
 */
void cbuild_obj_append_cflags(cbuild_obj_t *o, const char **cflags, size_t ncflags);
