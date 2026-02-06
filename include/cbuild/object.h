// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

typedef struct cbuild_obj cbuild_obj_t;

/**
 * Create an object file (.o) from a cfile (.c), with compile flags
 * from vararg.
 * Vararg must end in NULL.
 * On failure, returns null. otherwise, returns a pointer to struct,
 * which must be freed after no longer needed
 */
cbuild_obj_t *cbuild_obj_create(const char *cfile, ...);

void cbuild_obj_free(cbuild_obj_t *o);

/**
 * Append a flag to CFLAGS.
 * On errors, returns -1. otherwise zero.
 */
int cbuild_obj_append_cflags(cbuild_obj_t *o, const char *flag);
