#pragma once

#include <sys/types.h>
#include "cbuild/target.h"

struct cbuild_custom_target {
    const char *outpath;
    const char *inpath;
    cbuild_custom_target_func func;
};

