#!/bin/bash

set -o errexit -o nounset

if [ "$TRAVIS_BRANCH" != "master" ]
then
  echo "This commit was made against the $TRAVIS_BRANCH and not the master! Not going to deploy!"
  exit 0
fi

rev=$(git rev-parse --short HEAD)

doxygen Doxyfile.dox 2>&1

if [ -d "html" ] && [ -f "html/index.html" ]; then
    cd html

    git init
    git config user.name "Travis CI"
    git config user.email "travis@travis-ci.org"

    git remote add upstream "https://$GH_TOKEN@github.com/lnls-dig/openMMC.git"
    git fetch upstream
    git reset upstream/gh-pages

    touch .
    # Need to create a .nojekyll file to allow filenames starting with an underscore
    # to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
    # Presumably this is only needed when the SHORT_NAMES option in Doxygen is set
    # to NO, which it is by default. So creating the file just in case.
    echo "" > .nojekyll

    git add -A .
    git commit -m "Deploy code docs to GitHub Pages Travis build: $TRAVIS_BUILD_NUMBER" -m "Commit: $rev"
    git push -q upstream HEAD:gh-pages
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    echo 'Warning: Not going to push the documentation to GitHub!' >&2
    exit 1
fi
