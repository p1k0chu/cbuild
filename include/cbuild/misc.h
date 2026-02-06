// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

/**
 * Allocates a new string and replaces extension of S with EXT.
 *
 * The returned string must be freed with free()
 */
char *cbuild_changestrext(const char *s, const char *ext);
