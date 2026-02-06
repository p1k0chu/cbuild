#!/usr/bin/env bash

# this script is intended to be used by library consumers, you need to
# run it from the same directory as your build.c file. it generates a
# build.h header that defines a macro CBUILD_SELFCOMPILE_FLAGS, that
# you need to pass to cbuild_recompile_myself.

cbuild_dir=$(realpath $(dirname -- "${BASH_SOURCE[0]}"))

make -C "$cbuild_dir" || exit 1

build_c="$1"
if [[ -z "$build_c" ]]; then
    build_c="build.c"
fi

if [[ ! -f "$build_c" ]]; then
    echo "file \"$build_c\" does not exit" >&2
    exit 1
fi

build_exe="${build_c%.*}"
build_h="${build_exe}.h"

echo "Cbuild library path: $cbuild_dir"
echo "Build driver: $build_c"
echo "Generating header: $build_h"

cat > "$build_h" <<EOF
#define CBUILD_SELFCOMPILE_FLAGS \
    "-L$cbuild_dir", "-lcbuild", "-I${cbuild_dir}/include"
EOF

if [[ -z "$CC" ]]; then
    CC='gcc'
fi

if ! command -v "$CC" >/dev/null 2>&1
then
    echo "C compile not found. set CC env variable"
    exit 1
fi

flags="-L$cbuild_dir -lcbuild -I${cbuild_dir}/include"

echo "Compile command: $CC $build_c $flags -o $build_exe"

"$CC" "$build_c" $flags -o "$build_exe"
