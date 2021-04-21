#!/bin/bash

set -e

BUILD_TYPE=${BUILD_TYPE:-Debug}
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/../../

rm -rf build
tools/premake5 gmake2 -j4
tools/premake5 cmake
cd build
make
