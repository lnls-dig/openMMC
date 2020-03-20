#!/usr/bin/env bash

# help debug
set -x
set -e
set -o

if [ "${BUILD_ARTIFACT}" = "docs" ]; then
    # remove build directories, as we can't
    # use a file pattern in Doxyfile.dox file
    # like */build*/* as travis seems to be
    # expanding "*". This should be safe, as
    # we only run this when BUILD_ARTIFACT is set
    # to "docs" and not with "all_binaries", which
    # is the release BUILD_ARTIFACT
    rm -r $TRAVIS_BUILD_DIR/${BUILD_DIR}*

    # Now generate the documentation
    cd $TRAVIS_BUILD_DIR/docs
    doxygen Doxyfile.dox
    echo "" > html/.nojekyll
fi
