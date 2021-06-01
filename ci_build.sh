#!/usr/bin/env bash

# help debug
set -x
set -e
set -o

LANG=C
LC_ALL=C
export LANG LC_ALL

# Location of releases
set +e
GIT_DESCRIBE=$(git describe --tags --abbrev=10)
if [ $? -ne 0 ]; then
    GIT_DESCRIBE=devel
fi
set -e

# Declare all combinations for deploy all
# Note that the key is the name of the release object
declare -A BUILDS
BUILDS[afc-bpm-3.1-${GIT_DESCRIBE}]="\
     -DBOARD=afc-bpm \
     -DVERSION=3.1 \
     -DBOARD_RTM= \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo"
BUILDS[afc-timing-${GIT_DESCRIBE}]="\
     -DBOARD=afc-timing \
     -DVERSION= \
     -DBOARD_RTM=rtm-8sfp \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo"

TOP=$(pwd)

case "${BUILD_ARTIFACT}" in
    # Generate builds for all list items. Both "all_binaries"
    # and "docs" needs to build everyting and pass all tests.
    all_binaries|docs)
        for build in "${!BUILDS[@]}"; do
            echo "Generating build for:" ${build} && \
            mkdir -p ${BUILD_DIR}-${build} && \
            cd ${BUILD_DIR}-${build} && \
            cmake ../ ${BUILDS[${build}]}

            # For each build, generate the "full"
            # binary as well
            for target in all full_binary; do
                make ${target}
            done

            cd ${TOP}
        done

        # Extra step for "all_binaries" is to copy
        # the artifacts to a release folder
        if [ "${BUILD_ARTIFACT}" = "all_binaries" ]; then
            # Copy the generated files to the release folder
            mkdir -p ${RELEASE_DIR}
            for build in "${!BUILDS[@]}"; do
                cp ${BUILD_DIR}-${build}/out/openMMC.axf ${RELEASE_DIR}/openMMC-${build}.axf
                cp ${BUILD_DIR}-${build}/out/openMMC.bin ${RELEASE_DIR}/openMMC-${build}.bin
				cp ${BUILD_DIR}-${build}/out/bootloader.axf ${RELEASE_DIR}/bootloader-${build}.axf
                cp ${BUILD_DIR}-${build}/out/bootloader.bin ${RELEASE_DIR}/bootloader-${build}.bin
            done
        fi
        ;;

    # Regular build
    binary)
        mkdir -p ${BUILD_DIR} && \
        cd ${BUILD_DIR} && \
        cmake ../ \
        -DBOARD=${BOARD} \
        -DVERSION=${VERSION} \
        -DBOARD_RTM=${RTM} \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} && \
        make
        ;;

    *)
        echo "BUILD_ARTIFACT invalid: "${BUILD_ARTIFACT}
        exit 0
        ;;

esac
