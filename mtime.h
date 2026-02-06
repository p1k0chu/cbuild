// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

/**
 * Compares modified time of two files at paths p1 and p2.
 *
 * Returns:
 * - x > 0 when p1 is newer than p2, or p1 exists and p2 doesn't
 * - x < 0 when p2 is newer than p1, or p2 exists and p1 doesn't
 * - x == 0 when both files dont exit, or mtime is equal
 */
int cbuild__mtimecmp(const char *p1, const char *p2);
