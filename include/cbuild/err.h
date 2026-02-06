// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

enum cbuild_errno {
    /// just like stdlib's errno, zero indicates no errors
    CBUILD_ESUCCESS,
    /// error while using memory allocation functions
    CBUILD_EMALLOC,
    /// error while forking
    CBUILD_EFORK,
    /// error while execing
    CBUILD_EEXEC,
    /// error from snprintf, etc
    CBUILD_ESNPRINTF,
    /// snprintf didn't error but truncated the string
    CBUILD_ESNPRINTF_TRUNCATED
};

extern enum cbuild_errno cbuild_errno;
