#!/usr/bin/env bash

# help debug
set -x

TOP=$(pwd)

if [ "${DEPLOY}" = "all_binaries" ]; then
    mkdir dist
    export OPENMMC_DEPLOYMENT=dist/*

    # Build put our deploy targets into the release
    # directory.
    cd ${RELEASE_DIR}
    cp * ${TOP}/dist
    cd -

    # Generate hash sums
    cd dist
    md5sum ${TOP}/${RELEASE_DIR}/{*.bin,*.axf} > MD5SUMS
    sha1sum ${TOP}/${RELEASE_DIR}/{*.bin,*.axf} > SHA1SUMS
    cd -
fi

set +x
