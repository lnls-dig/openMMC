#!/usr/bin/env bash

# help debug
set -x
set -e
set -o

LANG=C
LC_ALL=C
export LANG LC_ALL

# Location of releases
GIT_DESCRIBE=$(git describe --tags --abbrev=10)

# Declare all combinations for deploy all
# Note that the key is the name of the release object
declare -A BUILDS
BUILDS[openMMC-afc-bpm-3.0-${GIT_DESCRIBE}]="\
     -DBOARD=afc-bpm \
     -DVERSION=3.0 \
     -DBOARD_RTM= \
     -DCMAKE_BUILD_TYPE=Release"
BUILDS[openMMC-afc-bpm-3.1-${GIT_DESCRIBE}]="\
     -DBOARD=afc-bpm \
     -DVERSION=3.1 \
     -DBOARD_RTM= \
     -DCMAKE_BUILD_TYPE=Release"
BUILDS[openMMC-afc-timing-${GIT_DESCRIBE}]="\
     -DBOARD=afc-timing \
     -DVERSION= \
     -DBOARD_RTM=rtm-8sfp \
     -DCMAKE_BUILD_TYPE=Release"
BUILDS[openMMC-debug-afc-bpm-3.0-${GIT_DESCRIBE}]="\
     -DBOARD=afc-bpm \
     -DVERSION=3.0 \
     -DBOARD_RTM= \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo"
BUILDS[openMMC-debug-afc-bpm-3.1-${GIT_DESCRIBE}]="\
     -DBOARD=afc-bpm \
     -DVERSION=3.1 \
     -DBOARD_RTM= \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo"
BUILDS[openMMC-debug-afc-timing-${GIT_DESCRIBE}]="\
     -DBOARD=afc-timing \
     -DVERSION= \
     -DBOARD_RTM=rtm-8sfp \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo"

TOP=$(pwd)

case "${DEPLOY}" in
    all)
        # Generate builds for all list items
        for build in "${!BUILDS[@]}"; do
            echo "Generating build for " ${build} && \
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

        # Copy the generated files to the release folder
        mkdir -p ${RELEASE_DIR}
        for build in "${!BUILDS[@]}"; do
            case "${build}" in
                openMMC-debug*)
                    cp build-${build}/out/openMMC.axf ${RELEASE_DIR}/openMMC-${build}.axf
                    ;;

                *)
                    cp build-${build}/out/openMMC.bin ${RELEASE_DIR}/openMMC-${build}.bin
                    cp build-${build}/out/openMMC_full.bin ${RELEASE_DIR}/openMMC-full-${build}.bin
                    ;;
            esac
        done

        ;;

    # Regular build
    *)
        mkdir -p ${BUILD_DIR} && \
        cd ${BUILD_DIR} && \
        cmake ../ \
        -DBOARD=${BOARD} \
        -DVERSION=${VERSION} \
        -DBOARD_RTM=${RTM} \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} && \
        make
        ;;

esac
