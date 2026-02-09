#!/usr/bin/env bash

# this script is intended to be used by library consumers, you need to
# run it from the same directory as your build.c file. it generates a
# build.h header that defines a macro CBUILD_SELFCOMPILE_FLAGS, that
# you need to pass to cbuild_recompile_myself.

# options:
# --nomake: dont run make in cbuild's root directory
# --static: prefer static version of the library

cbuild_dir=$(realpath $(dirname -- "${BASH_SOURCE[0]}"))

nomake=0
usestatic=0
build_c="build.c"

while [[ $# -gt 0 ]]; do
    case $1 in
	--nomake)
	    nomake=1
	    shift
	    ;;
	--static)
	    usestatic=1
	    shift
	    ;;
	-*|--*)
	    echo "Unknown option $1" >&2
	    exit 1
	    ;;
	*)
	    build_c="$1"
	    shift
	    ;;
    esac
done

if [[ ! -f "$build_c" ]]; then
    echo "file \"$build_c\" does not exit" >&2
    exit 1
fi

if [[ -z "$CC" ]]; then
    CC='gcc'
fi

if ! command -v "$CC" >/dev/null 2>&1
then
    echo "C compile not found. set CC env variable"
    exit 1
fi

if (( nomake == 0 )); then
    if (( usestatic == 1 )); then
	makeflag42="static"
    else
	makeflag42="shared"
    fi
    make -C "$cbuild_dir" $makeflag42 || exit 1
fi

build_exe="${build_c%.*}"
build_h="${build_exe}.h"

echo "Cbuild library path: $cbuild_dir"
echo "Build driver: $build_c"
echo "Generating header: $build_h"

flags="-L$cbuild_dir -I${cbuild_dir}/include"

if (( usestatic == 1 )); then
    flags="$flags -l:libcbuild.a"
    cat > "$build_h" <<EOF
#define CBUILD_SELFCOMPILE_FLAGS \
    "-L$cbuild_dir", "-I${cbuild_dir}/include", "-l:libcbuild.a"
EOF
else
    flags="$flags -l:libcbuild.so -Wl,-rpath=${cbuild_dir}"
    cat > "$build_h" <<EOF
#define CBUILD_SELFCOMPILE_FLAGS \
    "-L$cbuild_dir", "-I${cbuild_dir}/include", "-l:libcbuild.so", "-Wl,-rpath=$cbuild_dir"
EOF
fi

echo "$CC $build_c $flags -o $build_exe"

"$CC" "$build_c" $flags -o "$build_exe"
