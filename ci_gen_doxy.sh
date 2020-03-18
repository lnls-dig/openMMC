#!/usr/bin/env bash

# help debug
set -x
set -e
set -o

cd $TRAVIS_BUILD_DIR/docs && \
    ls -la && \
    ls -la ../ && \
    doxygen Doxyfile.dox && \
    echo "" > html/.nojekyll && \
    ls -la html
