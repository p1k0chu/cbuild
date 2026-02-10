// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "target.h"

#include <sys/types.h>

/// Print commands, but do not execute.
#define CBUILD_COMPILE_DRYRUN 1
/// Compile each target unconditionally.
#define CBUILD_COMPILE_FORCE (1 << 1)

/**
 * Compiles the target. If theres nothing to be done, might return
 * zero. Flags are described in <cbuild/compile.h> header.
 */
pid_t cbuild_target_compile(cbuild_target_t *target, int flags);

/**
 * Rebuilds your compile driver, if needed, and execs. sourcepath
 * should be value of __FILE__ macro for your build file.
 * argv is exact argv from your main. compile_flags are the same as
 * for cbuild_target_compile
 * varargs are both cflags and ldflags to compile yourself, at
 * minimum, you'd want to link against this library. varargs end in
 * null.
 *
 * Example:
 * cbuild_recompile_myself(__FILE__, argv, 0, "-lcbuild", NULL);
 */
void cbuild_recompile_myself(const char *sourcepath, char **argv, int compile_flags, ...);
