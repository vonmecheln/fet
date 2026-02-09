#!/bin/bash
set -euo pipefail

cd /build
rm -rf *
export LC_ALL=C.UTF-8
export LANG=C.UTF-8

# Use ccache for faster incremental builds
export CC="ccache gcc"
export CXX="ccache g++"
export CCACHE_DIR=${CCACHE_DIR:-/root/.ccache}

# Ask qmake to use the faster linker lld
qmake6 ../app/fet.pro QMAKE_LFLAGS+=" -fuse-ld=lld"

# Parallel build using all CPUs
make -j$(nproc)

cp ../app/translations . -r
