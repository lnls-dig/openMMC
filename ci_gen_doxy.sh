#!/usr/bin/env bash

# help debug
set -x
set -e
set -o

cd $TRAVIS_BUILD_DIR/docs && \
    doxygen Doxyfile.dox && \
    echo "" > html/.nojekyll
