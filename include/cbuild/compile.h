// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "object.h"
#include "target.h"

#include <sys/types.h>

pid_t cbuild_obj_compile(struct cbuild_obj *obj);

pid_t cbuild_target_compile(struct cbuild_target *target);

/**
 * Rebuilds your compile driver, if needed, and execs. sourcepath
 * should be value of __FILE__ macro for your build file.
 * argv is
 * exact argv from your main.
 * varargs are both cflags and ldflags to
 * compile yourself, at minimum, you'd want to link against this
 * library. varargs end in null.
 *
 * Example:
 * cbuild_recompile_myself(__FILE__, argv, "-lcbuild", NULL);
 */
void cbuild_recompile_myself(const char *sourcepath, char **argv, ...);
