// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

typedef struct cbuild_custom_target cbuild_custom_target_t;

typedef void (*cbuild_custom_target_func)(const char *outpath, const char *inpath, int flags, void*data);

/**
 * Define a custom target, which depends on file INPATH and produces
 * file OUTPATH by first forking and then calling FUNC.
 *
 * FUNC takes two arguments: outpath and inpath. It returns nothing,
 * and if fails, should call exit() with error code.  FUNC does not
 * need to check files modified time, its checked by cbuild before
 * running it. It also receives the compile flags, and a void*data,
 * data may be anything you like, or NULL, it doesn't do anything in
 * the library, its for you to pass custom data to your function.
 *
 * Your FUNC should also print what it's doing. for example, print the
 * command you are exec'ing. otherwise you'll never see if the target
 * was rebuilt or not.
 * Your function should also (at your option) respect flags it gets,
 * e.g. if dryrun is set - only print the message, and dont do
 * anything.
 */
cbuild_custom_target_t *cbuild_create_custom_target(const char *outpath,
                                                    const char *inpath,
                                                    cbuild_custom_target_func func,
                                                    void *data);
