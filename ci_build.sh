#!/usr/bin/env bash

# help debug
set -x
set -e
set -o

# Environment setup
LANG=C
LC_ALL=C
export LANG LC_ALL

mkdir build && \
    cd build && \
    cmake ../ \
        -DBOARD=${BOARD} \
        -DVERSION=${VERSION} \
        -DBOARD_RTM=${RTM} \
        -DCMAKE_BUILD_TYPE=Release && \
    make
