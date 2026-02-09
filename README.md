# cbuild

A small library that allows you to write build recipes in C.

API might change often because right now its not good enough.

# License

This library is licensed with LGPL-3.0-or-later. see `LICENSE` and
`COPYING.LESSER` for full text of the licenses.

# Usage

TLDR: full example (without error handling) at the bottom. also see
`build.c` for example usage. also see `examples/` directory for some
more examples.

First, you need to compile this library as a static lib. Clone the
repository and run `make`, this creates a `libcbuild.a` file in
project root. Either copy it to /usr/local/lib, and copy the include
dir to /usr/local/ (recursively), OR do whatever. its a normal
library. You can hardcode the absolute path, use env variables, add
this as a submodule in your git...

Then you need to use `-Lpath/to/lib`, `-lcbuild` and
`-Ipath/to/include` in your compile command (example command later)

To create an object file, you can use `cbuild_obj_create(source,
cflags..., NULL)`. it takes a path to c file and `const char *`
varargs to be used as cflags. varargs in all of the library functions
need to end in NULL. it returns NULL on errors, see each function's
documentation for their error behaviour. most return either NULL or -1
(like syscalls, if return < 0, then its error), and set `cbuild_errno`
value (of type `enum cbuild_errno`). A function that failed will never
set `cbuild_errno` to 0.

```c
#include <cbuild/cbuild.h>
cbuild_obj_t *foo_obj = cbuild_obj_create("foo.c", "-std=gnu23", "-Wall", "-O2", NULL);
```

If after the fact you want to add more cflags, you can use
`cbuild_obj_append_cflags`. notice how all obj related functions begin
with `cbuild_obj...`, for easy autocompletion. All
`cbuild_..._append_...` functions take an array of items.

```c
const char *wextra = "-Wextra";
cbuild_obj_append_cflags(foo_obj, &wextra, 1);

char *someflags[] = {"-std=c99", "-pedantic"};
cbuild_obj_append_cflags(foo_obj, someflags, 2);
```

You could compile it like that via `cbuild_obj_compile`, but you
can also create an executable using `cbuild_target_create`. it takes
the type of the target, output filename and a vararg of `cbuild_target_t *`'s.

There's multiple target types, but they all share the same base:
`cbuild_target_t`. It should be always safe to cast the specific types
to `cbuild_target_t` (you need to cast it sometimes).

Type of the target is one of `enum cbuild_target_type`, there's also 3
macros to create each type: `cbuild_create_executable`,
`cbuild_create_sharedlib`,`cbuild_create_staticlib`

```c
cbuild_executable_t *foo = cbuild_create_executable("foo", foo_obj, NULL);
```

Since you can't mix types for varargs (or you can, inconveniently),
you can only append ldflags via
`cbuild_executable_append_ldflags`. (its an alias for
`cbuild_link_target_append_ldflags`, same alias exists for other 2
types of link target)

```c
const char *ldlibs[] = {"-lm"};
cbuild_executable_append_ldflags(foo, ldlibs, sizeof(ldlibs) / sizeof(*ldlibs));
```

Now, to compile, use `cbuild_target_compile`. its going to compare the
timestamps of all the required files (if foo.c > foo.o, compile foo.o,
then if foo.o > foo, compile foo). All commands are printed to stdout.

It returns child process' pid, which you should `wait` for. (otherwise
the parent exists too early and kills its children). It may also
return 0 if the target is up to date, or -1 if an error occurred.

```c
// void* cast is simply shorter than cbuild_target_t*
pid_t cpid = cbuild_target_compile((void *)foo);
if (cpid < 0)
	exit(1);
if (cpid > 0)
	waitpid(cpid, NULL, 0);
```

Now compile your build driver. You can, for example, add this git repo
as a submodule, inside of `lib/cbuild`, run `make install` in it, and
link against it:

```c
gcc build.c -Llib/cbuild -lcbuild -Ilib/cbuild/include -o build
```

Now, if you run build, it should work.

It can also recompile itself, when its source file is newer than
executable. To do so, you need to call `cbuild_recompile_myself` on
the top of your `main(...)`. It also takes cflags and ldflags combined
as varargs. It needs, at minimum, flags to link against `cbuild`
library (just like in the command above). You can pass any other
flags, they should work.

```c
cbuild_recompile_myself(__FILE__, argv, "-Llib/cbuild", "-lcbuild", "-Ilib/cbuild/include", NULL);
```

if it decides to recompile itself, its going to print the commands
like usual, and its going to `exec` the new executable (that command
is also printed). If you want to force rebuild your compile driver,
you can update mtime of `build.c` and run `./build`

All compiler's commands, stdout and stderr are printed like usual,
emacs' parsing doesn't break.

You can use the `bootstrap.sh` script in the root of the project to,
well, bootstrap your build program. Instead of long and painful gcc
command the script compiles your build program for you, and generates
a header with a macro `CBUILD_SELFCOMPILE_FLAGS`, which you should
pass to `cbuild_recompile_myself` like this:

```c
cbuild_recompile_myself(__FILE__, argv, "-Wall", CBUILD_SELFCOMPILE_FLAGS, NULL);
```

---

Final program example (all error handling omitted for brevity):

```c
#include "build.h"

#include <cbuild/cbuild.h>
#include <wait.h>

int main(int argc, char **argv) {
    cbuild_recompile_myself(__FILE__,
                            argv,
							CBUILD_SELFCOMPILE_FLAGS,
                            NULL);

    cbuild_obj_t *foo_obj = cbuild_obj_create("foo.c", "-std=gnu23", "-Wall", "-Wextra", NULL);
    cbuild_executable_t *foo = cbuild_executable_create("foo", foo_obj, NULL);

	pid_t cpid = cbuild_target_compile((void *)foo);
	if (cpid > 0)
		waitpid(cpid, NULL, 0);
}
```

