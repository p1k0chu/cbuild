#define _GNU_SOURCE

#include "custom_target.h"

#include "cbuild/err.h"
#include "err.h"
#include "mtime.h"
#include "sys/stat.h"
#include "sys/wait.h"
#include "utils.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

cbuild_custom_target_t *cbuild_create_custom_target(const char *outpath,
                                                    const char *inpath,
                                                    cbuild_custom_target_func func) {
    cbuild_custom_target_t *t = calloc(1, sizeof(*t));
    if (t == NULL)
        CBUILD_RET_ERR(CBUILD_EMALLOC, NULL);

    t->outpath = outpath;
    t->inpath = inpath;
    t->func = func;

    return t;
}

pid_t cbuild_custom_target_compile(cbuild_custom_target_t *target) {
    if (cbuild__mtimecmp(target->outpath, target->inpath) >= 0)
        return 0;

    pid_t cpid = fork();
    if (cpid < 0) {
        CBUILD_RET_ERR(CBUILD_EFORK, -1);
    } else if (cpid > 0) {
        return cpid;
    }

    target->func(target->outpath, target->inpath);
    exit(0);
}

